// realesrgan implemented with ncnn library

#include "realesrgan.h"

#ifdef OV_VULKAN
#include "gpu.h"
#endif

#include <algorithm>
#include <vector>
#include <iostream>

#ifdef OV_VULKAN
static const uint32_t realesrgan_preproc_spv_data[] = {
    #include "shader/realesrgan_preproc.spv.hex.h"
};
static const uint32_t realesrgan_preproc_fp16s_spv_data[] = {
    #include "shader/realesrgan_preproc_fp16s.spv.hex.h"
};
static const uint32_t realesrgan_preproc_int8s_spv_data[] = {
    #include "shader/realesrgan_preproc_int8s.spv.hex.h"
};
static const uint32_t realesrgan_postproc_spv_data[] = {
    #include "shader/realesrgan_postproc.spv.hex.h"
};
static const uint32_t realesrgan_postproc_fp16s_spv_data[] = {
    #include "shader/realesrgan_postproc_fp16s.spv.hex.h"
};
static const uint32_t realesrgan_postproc_int8s_spv_data[] = {
    #include "shader/realesrgan_postproc_int8s.spv.hex.h"
};

static const uint32_t realesrgan_preproc_tta_spv_data[] = {
    #include "shader/realesrgan_preproc_tta.spv.hex.h"
};
static const uint32_t realesrgan_preproc_tta_fp16s_spv_data[] = {
    #include "shader/realesrgan_preproc_tta_fp16s.spv.hex.h"
};
static const uint32_t realesrgan_preproc_tta_int8s_spv_data[] = {
    #include "shader/realesrgan_preproc_tta_int8s.spv.hex.h"
};
static const uint32_t realesrgan_postproc_tta_spv_data[] = {
    #include "shader/realesrgan_postproc_tta.spv.hex.h"
};
static const uint32_t realesrgan_postproc_tta_fp16s_spv_data[] = {
    #include "shader/realesrgan_postproc_tta_fp16s.spv.hex.h"
};
static const uint32_t realesrgan_postproc_tta_int8s_spv_data[] = {
    #include "shader/realesrgan_postproc_tta_int8s.spv.hex.h"
};
#endif

RealEsrgan new_realesrgan(int gpuid, bool _tta_model) {
    return new ov::RealESRGAN(gpuid, _tta_model);
}

void destroy_realesrgan(RealEsrgan r) {
    delete static_cast<ov::RealESRGAN*>(r);
}

int realesrgan_load_model(RealEsrgan r, const char* root_path) {
    return static_cast<ov::RealESRGAN*>(r)->LoadModel(root_path);
}

int realesrgan_process(RealEsrgan r, const unsigned char* rgbdata, int img_width, int img_height, int scale, Bytes* output) {
    size_t total_size = img_width * img_height * scale * scale * 3;
#ifdef OV_VULKAN
    // const ncnn::Mat in = ncnn::Mat::from_pixels(rgbdata, ncnn::Mat::PIXEL_RGB,img_width, img_height); 
    const ncnn::Mat in = ncnn::Mat(img_width, img_height, (void *)rgbdata, (size_t)3, 3);
    ncnn::Mat out = ncnn::Mat(img_width * scale, img_height * scale, (size_t)3, 3);
    int ret = static_cast<ov::RealESRGAN*>(r)->process(in, out);
    if (ret != 0) {
        return ret;
    } 
    output->values = (unsigned char *)(malloc(total_size * sizeof(unsigned char)));
    memcpy(output->values, out.data, total_size * sizeof(unsigned char));
    output->length = total_size;
#endif
    return 0;
}

namespace ov {
RealESRGAN::RealESRGAN(int gpuid, bool _tta_mode)
{
    net = new ncnn::Net();
    initialized_ = false;
    net->opt.use_vulkan_compute = true;
    net->opt.use_fp16_packed = true;
    net->opt.use_fp16_storage = true;
    net->opt.use_fp16_arithmetic = false;
    net->opt.use_int8_storage = true;
    net->opt.use_int8_arithmetic = false;

#ifdef OV_VULKAN
    net->set_vulkan_device(gpuid);
    realesrgan_preproc = 0;
    realesrgan_postproc = 0;
    uint32_t heap_budget = ncnn::get_gpu_device(gpuid)->get_heap_budget();
    if (heap_budget > 1900)
        tilesize = 200;
    else if (heap_budget > 550)
        tilesize = 100;
    else if (heap_budget > 190)
        tilesize = 64;
    else
        tilesize = 32; 
#endif
    prepadding = 10;
    bicubic_4x = 0;
    tta_mode = _tta_mode;
}

RealESRGAN::~RealESRGAN()
{
#ifdef OV_VULKAN
    // cleanup preprocess and postprocess pipeline
    {
        delete realesrgan_preproc;
        delete realesrgan_postproc;
    }
#endif
    bicubic_4x->destroy_pipeline(net->opt);
    delete bicubic_4x;
    if (net) {
        net->clear();
    }
}

int RealESRGAN::LoadModel(const char* root_path)
{
    std::string param_file = std::string(root_path) + "/param";
	std::string model_file = std::string(root_path) + "/bin";
    if (net->load_param(param_file.c_str()) == -1 ||
        net->load_model(model_file.c_str()) == -1) {
        return 10000;
	}

    initialized_ = true;

#ifdef OV_VULKAN
    // initialize preprocess and postprocess pipeline
    {
        std::vector<ncnn::vk_specialization_type> specializations(1);
#if _WIN32
        specializations[0].i = 1;
#else
        specializations[0].i = 0;
#endif

        realesrgan_preproc = new ncnn::Pipeline(net->vulkan_device());
        realesrgan_preproc->set_optimal_local_size_xyz(32, 32, 3);

        realesrgan_postproc = new ncnn::Pipeline(net->vulkan_device());
        realesrgan_postproc->set_optimal_local_size_xyz(32, 32, 3);

        if (tta_mode)
        {
            if (net->opt.use_fp16_storage && net->opt.use_int8_storage)
                realesrgan_preproc->create(realesrgan_preproc_tta_int8s_spv_data, sizeof(realesrgan_preproc_tta_int8s_spv_data), specializations);
            else if (net->opt.use_fp16_storage)
                realesrgan_preproc->create(realesrgan_preproc_tta_fp16s_spv_data, sizeof(realesrgan_preproc_tta_fp16s_spv_data), specializations);
            else
                realesrgan_preproc->create(realesrgan_preproc_tta_spv_data, sizeof(realesrgan_preproc_tta_spv_data), specializations);

            if (net->opt.use_fp16_storage && net->opt.use_int8_storage)
                realesrgan_postproc->create(realesrgan_postproc_tta_int8s_spv_data, sizeof(realesrgan_postproc_tta_int8s_spv_data), specializations);
            else if (net->opt.use_fp16_storage)
                realesrgan_postproc->create(realesrgan_postproc_tta_fp16s_spv_data, sizeof(realesrgan_postproc_tta_fp16s_spv_data), specializations);
            else
                realesrgan_postproc->create(realesrgan_postproc_tta_spv_data, sizeof(realesrgan_postproc_tta_spv_data), specializations);
        }
        else
        {
            if (net->opt.use_fp16_storage && net->opt.use_int8_storage)
                realesrgan_preproc->create(realesrgan_preproc_int8s_spv_data, sizeof(realesrgan_preproc_int8s_spv_data), specializations);
            else if (net->opt.use_fp16_storage)
                realesrgan_preproc->create(realesrgan_preproc_fp16s_spv_data, sizeof(realesrgan_preproc_fp16s_spv_data), specializations);
            else
                realesrgan_preproc->create(realesrgan_preproc_spv_data, sizeof(realesrgan_preproc_spv_data), specializations);

            if (net->opt.use_fp16_storage && net->opt.use_int8_storage)
                realesrgan_postproc->create(realesrgan_postproc_int8s_spv_data, sizeof(realesrgan_postproc_int8s_spv_data), specializations);
            else if (net->opt.use_fp16_storage)
                realesrgan_postproc->create(realesrgan_postproc_fp16s_spv_data, sizeof(realesrgan_postproc_fp16s_spv_data), specializations);
            else
                realesrgan_postproc->create(realesrgan_postproc_spv_data, sizeof(realesrgan_postproc_spv_data), specializations);
        }
    }
#endif

    // bicubic 4x for alpha channel
    {
        bicubic_4x = ncnn::create_layer("Interp");
        bicubic_4x->vkdev = net->vulkan_device();

        ncnn::ParamDict pd;
        pd.set(0, 3);// bicubic
        pd.set(1, 4.f);
        pd.set(2, 4.f);
        bicubic_4x->load_param(pd);

        bicubic_4x->create_pipeline(net->opt);
    }

    return 0;
}

int RealESRGAN::process(const ncnn::Mat& inimage, ncnn::Mat& outimage) const
{
    const unsigned char* pixeldata = (const unsigned char*)inimage.data;
    const int w = inimage.w;
    const int h = inimage.h;
    const int channels = inimage.elempack;

    const int TILE_SIZE_X = tilesize;
    const int TILE_SIZE_Y = tilesize;

    ncnn::VkAllocator* blob_vkallocator = net->vulkan_device()->acquire_blob_allocator();
    ncnn::VkAllocator* staging_vkallocator = net->vulkan_device()->acquire_staging_allocator();

    ncnn::Option opt = net->opt;
    opt.blob_vkallocator = blob_vkallocator;
    opt.workspace_vkallocator = blob_vkallocator;
    opt.staging_vkallocator = staging_vkallocator;

    // each tile 100x100
    const int xtiles = (w + TILE_SIZE_X - 1) / TILE_SIZE_X;
    const int ytiles = (h + TILE_SIZE_Y - 1) / TILE_SIZE_Y;

    const size_t in_out_tile_elemsize = opt.use_fp16_storage ? 2u : 4u;

    //#pragma omp parallel for num_threads(2)
    for (int yi = 0; yi < ytiles; yi++)
    {
        const int tile_h_nopad = std::min((yi + 1) * TILE_SIZE_Y, h) - yi * TILE_SIZE_Y;

        int in_tile_y0 = std::max(yi * TILE_SIZE_Y - prepadding, 0);
        int in_tile_y1 = std::min((yi + 1) * TILE_SIZE_Y + prepadding, h);

        ncnn::Mat in;
        if (opt.use_fp16_storage && opt.use_int8_storage)
        {
            in = ncnn::Mat(w, (in_tile_y1 - in_tile_y0), (unsigned char*)pixeldata + in_tile_y0 * w * channels, (size_t)channels, 1);
        }
        else
        {
            if (channels == 3)
            {
#if _WIN32
                in = ncnn::Mat::from_pixels(pixeldata + in_tile_y0 * w * channels, ncnn::Mat::PIXEL_BGR2RGB, w, (in_tile_y1 - in_tile_y0));
#else
                in = ncnn::Mat::from_pixels(pixeldata + in_tile_y0 * w * channels, ncnn::Mat::PIXEL_RGB, w, (in_tile_y1 - in_tile_y0));
#endif
            }
            if (channels == 4)
            {
#if _WIN32
                in = ncnn::Mat::from_pixels(pixeldata + in_tile_y0 * w * channels, ncnn::Mat::PIXEL_BGRA2RGBA, w, (in_tile_y1 - in_tile_y0));
#else
                in = ncnn::Mat::from_pixels(pixeldata + in_tile_y0 * w * channels, ncnn::Mat::PIXEL_RGBA, w, (in_tile_y1 - in_tile_y0));
#endif
            }
        }

        ncnn::VkCompute cmd(net->vulkan_device());

        // upload
        ncnn::VkMat in_gpu;
        {
            cmd.record_clone(in, in_gpu, opt);

            if (xtiles > 1)
            {
                cmd.submit_and_wait();
                cmd.reset();
            }
        }

        int out_tile_y0 = std::max(yi * TILE_SIZE_Y, 0);
        int out_tile_y1 = std::min((yi + 1) * TILE_SIZE_Y, h);

        ncnn::VkMat out_gpu;
        if (opt.use_fp16_storage && opt.use_int8_storage)
        {
            out_gpu.create(w * scale, (out_tile_y1 - out_tile_y0) * scale, (size_t)channels, 1, blob_vkallocator);
        }
        else
        {
            out_gpu.create(w * scale, (out_tile_y1 - out_tile_y0) * scale, channels, (size_t)4u, 1, blob_vkallocator);
        }

        for (int xi = 0; xi < xtiles; xi++)
        {
            const int tile_w_nopad = std::min((xi + 1) * TILE_SIZE_X, w) - xi * TILE_SIZE_X;

            if (tta_mode)
            {
                // preproc
                ncnn::VkMat in_tile_gpu[8];
                ncnn::VkMat in_alpha_tile_gpu;
                {
                    // crop tile
                    int tile_x0 = xi * TILE_SIZE_X - prepadding;
                    int tile_x1 = std::min((xi + 1) * TILE_SIZE_X, w) + prepadding;
                    int tile_y0 = yi * TILE_SIZE_Y - prepadding;
                    int tile_y1 = std::min((yi + 1) * TILE_SIZE_Y, h) + prepadding;

                    in_tile_gpu[0].create(tile_x1 - tile_x0, tile_y1 - tile_y0, 3, in_out_tile_elemsize, 1, blob_vkallocator);
                    in_tile_gpu[1].create(tile_x1 - tile_x0, tile_y1 - tile_y0, 3, in_out_tile_elemsize, 1, blob_vkallocator);
                    in_tile_gpu[2].create(tile_x1 - tile_x0, tile_y1 - tile_y0, 3, in_out_tile_elemsize, 1, blob_vkallocator);
                    in_tile_gpu[3].create(tile_x1 - tile_x0, tile_y1 - tile_y0, 3, in_out_tile_elemsize, 1, blob_vkallocator);
                    in_tile_gpu[4].create(tile_y1 - tile_y0, tile_x1 - tile_x0, 3, in_out_tile_elemsize, 1, blob_vkallocator);
                    in_tile_gpu[5].create(tile_y1 - tile_y0, tile_x1 - tile_x0, 3, in_out_tile_elemsize, 1, blob_vkallocator);
                    in_tile_gpu[6].create(tile_y1 - tile_y0, tile_x1 - tile_x0, 3, in_out_tile_elemsize, 1, blob_vkallocator);
                    in_tile_gpu[7].create(tile_y1 - tile_y0, tile_x1 - tile_x0, 3, in_out_tile_elemsize, 1, blob_vkallocator);

                    if (channels == 4)
                    {
                        in_alpha_tile_gpu.create(tile_w_nopad, tile_h_nopad, 1, in_out_tile_elemsize, 1, blob_vkallocator);
                    }

                    std::vector<ncnn::VkMat> bindings(10);
                    bindings[0] = in_gpu;
                    bindings[1] = in_tile_gpu[0];
                    bindings[2] = in_tile_gpu[1];
                    bindings[3] = in_tile_gpu[2];
                    bindings[4] = in_tile_gpu[3];
                    bindings[5] = in_tile_gpu[4];
                    bindings[6] = in_tile_gpu[5];
                    bindings[7] = in_tile_gpu[6];
                    bindings[8] = in_tile_gpu[7];
                    bindings[9] = in_alpha_tile_gpu;

                    std::vector<ncnn::vk_constant_type> constants(13);
                    constants[0].i = in_gpu.w;
                    constants[1].i = in_gpu.h;
                    constants[2].i = in_gpu.cstep;
                    constants[3].i = in_tile_gpu[0].w;
                    constants[4].i = in_tile_gpu[0].h;
                    constants[5].i = in_tile_gpu[0].cstep;
                    constants[6].i = prepadding;
                    constants[7].i = prepadding;
                    constants[8].i = xi * TILE_SIZE_X;
                    constants[9].i = std::min(yi * TILE_SIZE_Y, prepadding);
                    constants[10].i = channels;
                    constants[11].i = in_alpha_tile_gpu.w;
                    constants[12].i = in_alpha_tile_gpu.h;

                    ncnn::VkMat dispatcher;
                    dispatcher.w = in_tile_gpu[0].w;
                    dispatcher.h = in_tile_gpu[0].h;
                    dispatcher.c = channels;
#ifdef OV_VULKAN
                    cmd.record_pipeline(realesrgan_preproc, bindings, constants, dispatcher);
#endif
                }

                // realesrgan
                ncnn::VkMat out_tile_gpu[8];
                for (int ti = 0; ti < 8; ti++)
                {
                    ncnn::Extractor ex = net->create_extractor();

                    ex.set_blob_vkallocator(blob_vkallocator);
                    ex.set_workspace_vkallocator(blob_vkallocator);
                    ex.set_staging_vkallocator(staging_vkallocator);

                    ex.input("data", in_tile_gpu[ti]);

                    ex.extract("output", out_tile_gpu[ti], cmd);

                    {
                        cmd.submit_and_wait();
                        cmd.reset();
                    }
                }

                ncnn::VkMat out_alpha_tile_gpu;
                if (channels == 4)
                {
                    if (scale == 1)
                    {
                        out_alpha_tile_gpu = in_alpha_tile_gpu;
                    }
                    if (scale == 4)
                    {
                        bicubic_4x->forward(in_alpha_tile_gpu, out_alpha_tile_gpu, cmd, opt);
                    }
                }

                // postproc
                {
                    std::vector<ncnn::VkMat> bindings(10);
                    bindings[0] = out_tile_gpu[0];
                    bindings[1] = out_tile_gpu[1];
                    bindings[2] = out_tile_gpu[2];
                    bindings[3] = out_tile_gpu[3];
                    bindings[4] = out_tile_gpu[4];
                    bindings[5] = out_tile_gpu[5];
                    bindings[6] = out_tile_gpu[6];
                    bindings[7] = out_tile_gpu[7];
                    bindings[8] = out_alpha_tile_gpu;
                    bindings[9] = out_gpu;

                    std::vector<ncnn::vk_constant_type> constants(13);
                    constants[0].i = out_tile_gpu[0].w;
                    constants[1].i = out_tile_gpu[0].h;
                    constants[2].i = out_tile_gpu[0].cstep;
                    constants[3].i = out_gpu.w;
                    constants[4].i = out_gpu.h;
                    constants[5].i = out_gpu.cstep;
                    constants[6].i = xi * TILE_SIZE_X * scale;
                    constants[7].i = std::min(TILE_SIZE_X * scale, out_gpu.w - xi * TILE_SIZE_X * scale);
                    constants[8].i = prepadding * scale;
                    constants[9].i = prepadding * scale;
                    constants[10].i = channels;
                    constants[11].i = out_alpha_tile_gpu.w;
                    constants[12].i = out_alpha_tile_gpu.h;

                    ncnn::VkMat dispatcher;
                    dispatcher.w = std::min(TILE_SIZE_X * scale, out_gpu.w - xi * TILE_SIZE_X * scale);
                    dispatcher.h = out_gpu.h;
                    dispatcher.c = channels;
#ifdef OV_VULKAN
                    cmd.record_pipeline(realesrgan_postproc, bindings, constants, dispatcher);
#endif
                }
            }
            else
            {
                // preproc
                ncnn::VkMat in_tile_gpu;
                ncnn::VkMat in_alpha_tile_gpu;
                {
                    // crop tile
                    int tile_x0 = xi * TILE_SIZE_X - prepadding;
                    int tile_x1 = std::min((xi + 1) * TILE_SIZE_X, w) + prepadding;
                    int tile_y0 = yi * TILE_SIZE_Y - prepadding;
                    int tile_y1 = std::min((yi + 1) * TILE_SIZE_Y, h) + prepadding;

                    in_tile_gpu.create(tile_x1 - tile_x0, tile_y1 - tile_y0, 3, in_out_tile_elemsize, 1, blob_vkallocator);

                    if (channels == 4)
                    {
                        in_alpha_tile_gpu.create(tile_w_nopad, tile_h_nopad, 1, in_out_tile_elemsize, 1, blob_vkallocator);
                    }

                    std::vector<ncnn::VkMat> bindings(3);
                    bindings[0] = in_gpu;
                    bindings[1] = in_tile_gpu;
                    bindings[2] = in_alpha_tile_gpu;

                    std::vector<ncnn::vk_constant_type> constants(13);
                    constants[0].i = in_gpu.w;
                    constants[1].i = in_gpu.h;
                    constants[2].i = in_gpu.cstep;
                    constants[3].i = in_tile_gpu.w;
                    constants[4].i = in_tile_gpu.h;
                    constants[5].i = in_tile_gpu.cstep;
                    constants[6].i = prepadding;
                    constants[7].i = prepadding;
                    constants[8].i = xi * TILE_SIZE_X;
                    constants[9].i = std::min(yi * TILE_SIZE_Y, prepadding);
                    constants[10].i = channels;
                    constants[11].i = in_alpha_tile_gpu.w;
                    constants[12].i = in_alpha_tile_gpu.h;

                    ncnn::VkMat dispatcher;
                    dispatcher.w = in_tile_gpu.w;
                    dispatcher.h = in_tile_gpu.h;
                    dispatcher.c = channels;

#ifdef OV_VULKAN
                    cmd.record_pipeline(realesrgan_preproc, bindings, constants, dispatcher);
#endif
                }

                // realesrgan
                ncnn::VkMat out_tile_gpu;
                {
                    ncnn::Extractor ex = net->create_extractor();

                    ex.set_blob_vkallocator(blob_vkallocator);
                    ex.set_workspace_vkallocator(blob_vkallocator);
                    ex.set_staging_vkallocator(staging_vkallocator);

                    ex.input("data", in_tile_gpu);

                    ex.extract("output", out_tile_gpu, cmd);
                }

                ncnn::VkMat out_alpha_tile_gpu;
                if (channels == 4)
                {
                    if (scale == 1)
                    {
                        out_alpha_tile_gpu = in_alpha_tile_gpu;
                    }
                    if (scale == 4)
                    {
                        bicubic_4x->forward(in_alpha_tile_gpu, out_alpha_tile_gpu, cmd, opt);
                    }
                }

                // postproc
                {
                    std::vector<ncnn::VkMat> bindings(3);
                    bindings[0] = out_tile_gpu;
                    bindings[1] = out_alpha_tile_gpu;
                    bindings[2] = out_gpu;

                    std::vector<ncnn::vk_constant_type> constants(13);
                    constants[0].i = out_tile_gpu.w;
                    constants[1].i = out_tile_gpu.h;
                    constants[2].i = out_tile_gpu.cstep;
                    constants[3].i = out_gpu.w;
                    constants[4].i = out_gpu.h;
                    constants[5].i = out_gpu.cstep;
                    constants[6].i = xi * TILE_SIZE_X * scale;
                    constants[7].i = std::min(TILE_SIZE_X * scale, out_gpu.w - xi * TILE_SIZE_X * scale);
                    constants[8].i = prepadding * scale;
                    constants[9].i = prepadding * scale;
                    constants[10].i = channels;
                    constants[11].i = out_alpha_tile_gpu.w;
                    constants[12].i = out_alpha_tile_gpu.h;

                    ncnn::VkMat dispatcher;
                    dispatcher.w = std::min(TILE_SIZE_X * scale, out_gpu.w - xi * TILE_SIZE_X * scale);
                    dispatcher.h = out_gpu.h;
                    dispatcher.c = channels;

#ifdef OV_VULKAN
                    cmd.record_pipeline(realesrgan_postproc, bindings, constants, dispatcher);
#endif
                }
            }

            if (xtiles > 1)
            {
                cmd.submit_and_wait();
                cmd.reset();
            }

            fprintf(stderr, "%.2f%%\n", (float)(yi * xtiles + xi) / (ytiles * xtiles) * 100);
        }

        // download
        {
            ncnn::Mat out;

            if (opt.use_fp16_storage && opt.use_int8_storage)
            {
                out = ncnn::Mat(out_gpu.w, out_gpu.h, (unsigned char*)outimage.data + yi * scale * TILE_SIZE_Y * w * scale * channels, (size_t)channels, 1);
            }

            cmd.record_clone(out_gpu, out, opt);

            cmd.submit_and_wait();

            if (!(opt.use_fp16_storage && opt.use_int8_storage))
            {
                if (channels == 3)
                {
#if _WIN32
                    out.to_pixels((unsigned char*)outimage.data + yi * scale * TILE_SIZE_Y * w * scale * channels, ncnn::Mat::PIXEL_RGB2BGR);
#else
                    out.to_pixels((unsigned char*)outimage.data + yi * scale * TILE_SIZE_Y * w * scale * channels, ncnn::Mat::PIXEL_RGB);
#endif
                }
                if (channels == 4)
                {
#if _WIN32
                    out.to_pixels((unsigned char*)outimage.data + yi * scale * TILE_SIZE_Y * w * scale * channels, ncnn::Mat::PIXEL_RGBA2BGRA);
#else
                    out.to_pixels((unsigned char*)outimage.data + yi * scale * TILE_SIZE_Y * w * scale * channels, ncnn::Mat::PIXEL_RGBA);
#endif
                }
            }
        }
    }

#ifdef OV_VULKAN
    net->vulkan_device()->reclaim_blob_allocator(blob_vkallocator);
    net->vulkan_device()->reclaim_staging_allocator(staging_vkallocator);
#endif

    return 0;
}
}
