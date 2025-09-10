#include <gtest/gtest.h>

#include "Lib/BaseType.h"
#include "matrix3d.h"

#include "Common/XferCRC.h"

/*


-exec p turnPos
$7 = {x = -249.418137, y = 1604.44214, z = 18.75}
-exec p relAngle
$8 = -0.841932654
-exec p trackPosDelta
$9 = {x = 163.638657, y = -162.13501, z = 0}

Original matrix of object
$1 = {Row = {{X = 1, Y = 0, Z = 0, W = -209.418137}, {X = 0, Y = 1, Z = 0, W = 1604.44214}, {X = 0, Y = 0, Z = 1, W = 18.75}}}
*/

TEST(Matrix, CRC)
{
  Matrix3D mtxObj;
  mtxObj.Set(1,0,0,-209.418137f,
             0,1,0,1604.44214f,
             0,0,1,18.75f);
  Coord3D turnPos = {-249.418137f, 1604.44214f, 18.75f};
  Coord3D trackPosDelta = {163.638657f, -162.13501f, 0};
  Real relAngle = -0.841932654f;

	Matrix3D mtx;
	Matrix3D tmp(1);
	tmp.Translate(turnPos.x, turnPos.y, 0);
	tmp.Translate(trackPosDelta.x, trackPosDelta.y, 0);
	tmp.In_Place_Pre_Rotate_Z(relAngle );

	tmp.Translate(-turnPos.x, -turnPos.y, 0);


	mtx.mul(tmp, mtxObj);
  

  XferCRC crc;
  crc.xferMatrix3D(&mtx);
  EXPECT_EQ(crc.getCRC(), 0xC9515EA7);
}