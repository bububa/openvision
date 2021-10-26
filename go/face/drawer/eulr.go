package drawer

import "math"

// EulrToAxisPoints returns a transformation roll/pitch/yaw to Axis in 2d
// intrinsic rotations around the (local) axis Y, X, Z, in that order.
func EulrToAxisPoints(yaw float64, pitch float64, roll float64, distance float64) [3][2]float64 {
	pitch *= math.Pi / 180
	yaw *= -1 * math.Pi / 180
	roll *= math.Pi / 180

	cYaw := math.Cos(yaw)
	cPitch := math.Cos(pitch)
	cRoll := math.Cos(roll)

	sYaw := math.Sin(yaw)
	sPitch := math.Sin(pitch)
	sRoll := math.Sin(roll)

	xAxisX := distance * cYaw * cRoll
	xAxisY := distance * (cPitch*sRoll + cRoll*sPitch*sYaw)

	yAxisX := distance * -1 * cYaw * sRoll
	yAxisY := distance * (cPitch*cRoll - sPitch*sYaw*sRoll)

	zAxisX := distance * sYaw
	zAxisY := distance * -1 * cYaw * sPitch

	return [3][2]float64{
		{xAxisX, xAxisY},
		{yAxisX, yAxisY},
		{zAxisX, zAxisY},
	}
}
