#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include "CImage.h"
#include "fiv_vrml.h"

struct coord
{
  double x, y;
  double xx, yy, zz;
};

struct coord3d
{
  double xx, yy, zz;
};

void LoadConfig(float K[3][3], 
		std::string ConfigName,
		std::string RefImName);

void LoadConfig(float K[3][3], 
		float R[3][3],
		float T[3],
		std::string ConfigName,
		std::string RefImName);

// Compute the determinant of a 3*3 matrix
// [ a1 a2 a3;
//   a4 a5 a6;
//   a7 a8 a9; ]
static inline float det3x3(float a1, float a2, float a3, float a4, float a5,
                           float a6, float a7, float a8, float a9)
{
  return a1*a5*a9 + a4*a8*a3 + a7*a2*a6
    - a7*a5*a3 - a4*a2*a9 - a1*a8*a6 ;
}


// Inverse matrix of a 3x3 matrix a
static inline void inv_3x3(float a[3][3], float inv[3][3])
{
  float det = det3x3(a[0][0], a[0][1], a[0][2], a[1][0], a[1][1],
                     a[1][2], a[2][0], a[2][1], a[2][2]);
        
  inv[0][0] =   (a[1][1]*a[2][2] - a[2][1]*a[1][2])/det;
  inv[1][0] = - (a[1][0]*a[2][2] - a[2][0]*a[1][2])/det;
  inv[2][0] =   (a[1][0]*a[2][1] - a[2][0]*a[1][1])/det;
        
  inv[0][1] = - (a[0][1]*a[2][2] - a[2][1]*a[0][2])/det;
  inv[1][1] =   (a[0][0]*a[2][2] - a[2][0]*a[0][2])/det;
  inv[2][1] = - (a[0][0]*a[2][1] - a[2][0]*a[0][1])/det;
        
  inv[0][2] =   (a[0][1]*a[1][2] - a[1][1]*a[0][2])/det;
  inv[1][2] = - (a[0][0]*a[1][2] - a[1][0]*a[0][2])/det;
  inv[2][2] =   (a[0][0]*a[1][1] - a[1][0]*a[0][1])/det;
}


// c = a*b;
static inline void MatMul3_3x3_1(float a[3][3], float b[3], float c[3])
{
  c[0] = a[0][0]*b[0] + a[0][1]*b[1] + a[0][2]*b[2];
  c[1] = a[1][0]*b[0] + a[1][1]*b[1] + a[1][2]*b[2];
  c[2] = a[2][0]*b[0] + a[2][1]*b[1] + a[2][2]*b[2];
}


int main(int argc, char** argv)
{
  int  ArgCount = 1;
	  
  std::string ConfigName = argv[ArgCount++];
  std::string InputName  = argv[ArgCount++];
  std::string OutputName = argv[ArgCount++];
  std::string RefImPath  = argv[ArgCount++];
  std::string RefImName  = argv[ArgCount++];

  int ImWidth    = atoi(argv[ArgCount++]);
  int ImHeight   = atoi(argv[ArgCount++]);

  float MinDepth = atof(argv[ArgCount++]);
  float MaxDepth = atof(argv[ArgCount++]);

  int ClipLeft  = atoi(argv[ArgCount++]);
  int ClipRight = atoi(argv[ArgCount++]);
  int ClipUp    = atoi(argv[ArgCount++]);
  int ClipDown  = atoi(argv[ArgCount++]);

  CImage RefIm(RefImPath+RefImName);
  RefIm.ReadPPM();

  // Clip out a small part of depth range
  const float sigma = 0.02;
  MinDepth = MinDepth + sigma*(MaxDepth - MinDepth);
  MaxDepth = MaxDepth - sigma*(MaxDepth - MinDepth);

  float K[3][3], InvK[3][3], R[3][3], InvR[3][3], T[3];

  // Load configuration file
  LoadConfig(K, R, T, ConfigName, RefImName);

  // Inverse matrix of K
  inv_3x3(K, InvK);

  // Inverse matrix of R
  inv_3x3(R, InvR);

  float* InputBuf  = new float[ImWidth*ImHeight];
  coord* OutputBuf = new coord[ImWidth*ImHeight];
  
  coord3d* PtCloud = new coord3d[ImWidth*ImHeight];

  // Open depth map
  std::ifstream ifm;
  ifm.open(InputName.c_str());
  ifm.read((char*)InputBuf, ImWidth*ImHeight*sizeof(float));
  ifm.close();

  int OutputIdx = 0;
  int InputIdx  = 0;

  double* PtColor = new double[ImWidth*ImHeight*3];

  for(int i=0; i<ImHeight; i++)
    {
      for(int j=0; j<ImWidth; j++)
	{
	  if(InputBuf[InputIdx]<=MinDepth+0.2    || 
	     InputBuf[InputIdx]>=MaxDepth-0.2    ||
	     i<ClipUp   || i>ImHeight-ClipDown   ||
	     j<ClipLeft || j>ImWidth-ClipRight)
	    {
	      InputIdx++;
	      continue;
	    }

	  PtColor[OutputIdx*3  ] = (RefIm.ImageData[InputIdx*3  ])/255.0;
	  PtColor[OutputIdx*3+1] = (RefIm.ImageData[InputIdx*3+1])/255.0;
	  PtColor[OutputIdx*3+2] = (RefIm.ImageData[InputIdx*3+2])/255.0;
	  
	  float PixPos[3] = {j ,i, 1};
	  float CamPos[3], WorldPos[3];
	  MatMul3_3x3_1(InvK, PixPos, CamPos);

	  CamPos[0]*=InputBuf[InputIdx];
	  CamPos[1]*=InputBuf[InputIdx];
	  CamPos[2]*=InputBuf[InputIdx];

	  // inv(R)*(CamPos - T)
	  CamPos[0] -= T[0];
	  CamPos[1] -= T[1];
	  CamPos[2] -= T[2];

	  MatMul3_3x3_1(InvR, CamPos, WorldPos);

	  OutputBuf[OutputIdx].x  = j;
	  OutputBuf[OutputIdx].y  = i;
	  OutputBuf[OutputIdx].xx = WorldPos[0];
	  OutputBuf[OutputIdx].yy = WorldPos[1];
	  OutputBuf[OutputIdx].zz = WorldPos[2];

	  PtCloud[OutputIdx].xx = OutputBuf[OutputIdx].xx;
	  PtCloud[OutputIdx].yy = OutputBuf[OutputIdx].yy;
	  PtCloud[OutputIdx].zz = OutputBuf[OutputIdx].zz;

	  InputIdx++;
	  OutputIdx++;
	}
    }
  
  std::cout << "Number of input points: "  << InputIdx  << std::endl;
  std::cout << "Number of output points: " << OutputIdx << std::endl; 

  fiv_vrml vrml;
  vrml.begin_sub_file("test.wrl");
  vrml.add_points((double*)PtCloud, PtColor, OutputIdx);
  vrml.end_sub_file();

  std::ofstream ofm;
  ofm.open(OutputName.c_str(), std::ios::trunc);
  ofm.write((char*)(&OutputIdx), sizeof(int));
  ofm.write((char*)OutputBuf, OutputIdx*sizeof(coord));
  ofm.close();
  
  delete[] InputBuf;
  delete[] OutputBuf;
  delete[] PtCloud;
  delete[] PtColor;

  return 0;
}

void LoadConfig(float K[3][3], 
		float R[3][3],
		float T[3],
		std::string ConfigName,
		std::string RefImName)
{
  // Open config file
  std::ifstream ifm;
  ifm.open(ConfigName.c_str());
  
  // Exit if the file cannot be read
  if(!ifm.good())
    {
      std::cerr << "Cannot open " << ConfigName << " !" << std::endl;
      exit(1);
    }
    
  int ImNumInConfigFile;
  float scale, temp;

  // Read number of input images, and scale
  ifm >> ImNumInConfigFile >> scale;

  for(int i=0; i<ImNumInConfigFile; i++)
    {
      std::string ImageName;
      
      // Read image name from config file
      ifm >> ImageName;            

      // Load parameters
      ifm >>  K[0][0] >> K[0][1] >> K[0][2]
	  >>  K[1][0] >> K[1][1] >> K[1][2]
	  >>  K[2][0] >> K[2][1] >> K[2][2];
      
      K[0][0] = K[0][0]*scale;
      K[0][1] = K[0][1]*scale;
      K[0][2] = K[0][2]*scale;
      
      K[1][0] = K[1][0]*scale;
      K[1][1] = K[1][1]*scale;
      K[1][2] = K[1][2]*scale;

      ifm >>  R[0][0] >> R[0][1] >> R[0][2]
	  >>  R[1][0] >> R[1][1] >> R[1][2]
	  >>  R[2][0] >> R[2][1] >> R[2][2];
			
      ifm >>  T[0]    >> T[1]    >> T[2];

      if(ImageName.compare(RefImName)==0)
	{
	  std::cout << RefImName << std::endl;

	  std::cout << K[0][0] << " " << K[0][1] << " " << K[0][2] << std::endl
		    << K[1][0] << " " << K[1][1] << " " << K[1][2] << std::endl
		    << K[2][0] << " " << K[2][1] << " " << K[2][2] << std::endl;

	  std::cout << R[0][0] << " " << R[0][1] << " " << R[0][2] << std::endl
		    << R[1][0] << " " << R[1][1] << " " << R[1][2] << std::endl
		    << R[2][0] << " " << R[2][1] << " " << R[2][2] << std::endl;

	  std::cout << T[0] << " " << T[1] << " " << T[2] << std::endl;

 	  break;
	}
    }

  ifm.close();
}
