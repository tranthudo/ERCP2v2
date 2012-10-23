// Obj.h: interface for the CObj class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(_OBJ_H_)
#define _OBJ_H_

#include "DataStruct.h"
#include <GL/gl.h>
#include <GL/glut.h>

class CObj  
{
	typedef struct
	{
		int		FaceNum;
		int*	FaceIndex;
	}	NeighborFace;

public:
	//object의 중심점
	vec3d   m_MidPoint;

protected:
	vec3d*	m_Point;
	vec3d*	m_InitialPoint;
	vec3d*  m_PrePoint;

	vec3i*	m_Face;
	vec3d*	m_FaceNormal;
	vec3d*	m_PointNormal;
	NeighborFace*	m_PointNeiFace;

	int		m_PointNum;
	int		m_FaceNum;

public:
	CObj()
	{
		m_InitialPoint=NULL;
		m_Point=NULL;
		m_PrePoint=NULL;
		m_Face=NULL;
		m_FaceNormal=NULL;
		m_PointNormal=NULL;
		m_PointNeiFace=NULL;

		m_PointNum=0;
		m_FaceNum=0;
	};

	~CObj()
	{
		if(m_InitialPoint){delete [] m_InitialPoint; m_InitialPoint=NULL;}
		if(m_Point){delete [] m_Point; m_Point=NULL;}
		if(m_PrePoint){delete [] m_PrePoint; m_PrePoint=NULL;}
		if(m_Face){delete [] m_Face; m_Face=NULL;}
		if(m_FaceNormal){delete [] m_FaceNormal; m_FaceNormal=NULL;}
		if(m_PointNormal){delete [] m_PointNormal; m_PointNormal=NULL;}
		DeleteNeiFace();
	};

	void DeleteNeiFace()
	{
		if(m_PointNeiFace)
		{
			for(int i=0;i<m_PointNum;i++)
			{
				if(m_PointNeiFace[i].FaceIndex)
				{
					delete [] m_PointNeiFace[i].FaceIndex;
					m_PointNeiFace[i].FaceIndex=NULL;
				}
			}
			delete [] m_PointNeiFace;
			m_PointNeiFace=NULL;
		}
	};

	int ReadObjData(char* filename)
	{
		FILE *fp;
		int i;

		fp = fopen(filename,"r");

		fscanf(fp,"%d\n",&m_PointNum);
		m_Point = new vec3d[m_PointNum];
		m_PrePoint = new vec3d[m_PointNum];
		m_InitialPoint = new vec3d[m_PointNum];

		for(i=0;i<m_PointNum;i++)
		{
			fscanf(fp,"%lf %lf %lf\n",&m_Point[i].x, &m_Point[i].y, &m_Point[i].z);
			m_InitialPoint[i]=m_Point[i];
			m_PrePoint[i]=m_Point[i];
		}

		fscanf(fp,"%d\n",&m_FaceNum);
		m_Face = new vec3i[m_FaceNum];

		for(i=0;i<m_FaceNum;i++)
			fscanf(fp,"%d %d %d\n",&m_Face[i].ii, &m_Face[i].jj, &m_Face[i].kk);

		//center point 계산
		ComputeCenterPoint();

		//face normal vector 계산
		ComputeFaceNormalVector();

		//point normal vector 계산
		ComputePointNormalVector();

		fclose(fp);
		return 1;
	};

	int WriteObjData(char* filename)
	{
		FILE *fp;
		int i;

		fp = fopen(filename,"w");

		fprintf(fp,"%d\n",m_PointNum);

		for(i=0;i<m_PointNum;i++)
			fprintf(fp,"%f %f %f\n",m_Point[i].x, m_Point[i].y, m_Point[i].z);

		fprintf(fp,"%d\n",m_FaceNum);

		for(i=0;i<m_FaceNum;i++)
			fprintf(fp,"%d %d %d\n",m_Face[i].ii, m_Face[i].jj, m_Face[i].kk);

		for(i=0;i<m_FaceNum;i++)
			fprintf(fp,"%f %f %f\n",m_FaceNormal[i].x, m_FaceNormal[i].x, m_FaceNormal[i].x);

		fclose(fp);
		return 1;
	};

	int ComputeFaceNormalVector()
	{
		int i;
		m_FaceNormal = new vec3d[m_FaceNum];
		vec3d vector1, vector2;

		for(i=0;i<m_FaceNum;i++)
		{
			vector1=m_Point[m_Face[i].jj]-m_Point[m_Face[i].ii]; 
			vector2=m_Point[m_Face[i].kk]-m_Point[m_Face[i].ii]; 
			m_FaceNormal[i] = vector1.cross(vector2);
			m_FaceNormal[i].normalize();
		}
		return 1;
	};

	int UpdateFaceNormalVector()
	{
		vec3d vector1, vector2;

		for(int i=0;i<m_FaceNum;i++)
		{
			vector1=m_Point[m_Face[i].jj]-m_Point[m_Face[i].ii]; 
			vector2=m_Point[m_Face[i].kk]-m_Point[m_Face[i].ii]; 
			m_FaceNormal[i] = vector1.cross(vector2);
			m_FaceNormal[i].normalize();
		}
		return 1;
	};

	int ComputePointNormalVector()
	{
		int i,j;
		m_PointNeiFace=new NeighborFace[m_PointNum];
		m_PointNormal=new vec3d[m_PointNum];

		//initialize
		for(i=0;i<m_PointNum;i++)
		{
			m_PointNeiFace[i].FaceNum=0;
			m_PointNeiFace[i].FaceIndex=NULL;
		}

		//point 주변의 면들의 개수를 저장한다
		for(i=0;i<m_FaceNum;i++)
		{
			m_PointNeiFace[m_Face[i].ii].FaceNum++;
			m_PointNeiFace[m_Face[i].jj].FaceNum++;
			m_PointNeiFace[m_Face[i].kk].FaceNum++;
		}

		//메모리 할당
		for(i=0;i<m_PointNum;i++)
		{
			m_PointNeiFace[i].FaceIndex=new int[m_PointNeiFace[i].FaceNum];
			m_PointNeiFace[i].FaceNum=0;
		}

		//한 point 주변의 면들의 face index를 저장한다
		for(i=0;i<m_FaceNum;i++)
		{
			m_PointNeiFace[m_Face[i].ii].FaceIndex[m_PointNeiFace[m_Face[i].ii].FaceNum]=i;
			m_PointNeiFace[m_Face[i].ii].FaceNum++;

			m_PointNeiFace[m_Face[i].jj].FaceIndex[m_PointNeiFace[m_Face[i].jj].FaceNum]=i;
			m_PointNeiFace[m_Face[i].jj].FaceNum++;

			m_PointNeiFace[m_Face[i].kk].FaceIndex[m_PointNeiFace[m_Face[i].kk].FaceNum]=i;
			m_PointNeiFace[m_Face[i].kk].FaceNum++;
		}

		//주변면들의 normal vector를 평균하여 point normal vector를 계산한다
		for(i=0;i<m_PointNum;i++)
		{
			for(j=0;j<m_PointNeiFace[i].FaceNum;j++)
				m_PointNormal[i]=m_PointNormal[i]+m_FaceNormal[m_PointNeiFace[i].FaceIndex[j]];
			m_PointNormal[i]=m_PointNormal[i]/(float)m_PointNeiFace[i].FaceNum;
			m_PointNormal[i].normalize();
		}
		return 1;
	};

	int UpdatePointNormalVector()
	{
		//주변면들의 normal vector를 평균하여 point normal vector를 계산한다
		for(int i=0;i<m_PointNum;i++)
		{
			m_PointNormal[i].setValue(0,0,0);
			for(int j=0;j<m_PointNeiFace[i].FaceNum;j++)
				m_PointNormal[i]=m_PointNormal[i]+m_FaceNormal[m_PointNeiFace[i].FaceIndex[j]];
			m_PointNormal[i]=m_PointNormal[i]/(float)m_PointNeiFace[i].FaceNum;
			m_PointNormal[i].normalize();
		}
		return 1;
	};

	int DrawObject(bool mode, vec3d color) 
	{
		glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);
		glColor3f(color.x,color.y,color.z);

		int i;
		if(mode)
		{
			glBegin(GL_TRIANGLES);
			for(i=0;i<m_FaceNum;i++)
			{
				glNormal3f(m_PointNormal[m_Face[i].ii].x,m_PointNormal[m_Face[i].ii].y,m_PointNormal[m_Face[i].ii].z);
				glVertex3f(m_Point[m_Face[i].ii].x, m_Point[m_Face[i].ii].y, m_Point[m_Face[i].ii].z);
				glNormal3f(m_PointNormal[m_Face[i].jj].x,m_PointNormal[m_Face[i].jj].y,m_PointNormal[m_Face[i].jj].z);
				glVertex3f(m_Point[m_Face[i].jj].x, m_Point[m_Face[i].jj].y, m_Point[m_Face[i].jj].z);
				glNormal3f(m_PointNormal[m_Face[i].kk].x,m_PointNormal[m_Face[i].kk].y,m_PointNormal[m_Face[i].kk].z);
				glVertex3f(m_Point[m_Face[i].kk].x, m_Point[m_Face[i].kk].y, m_Point[m_Face[i].kk].z);
			}
			glEnd();
		}
		else
		{
			glBegin(GL_TRIANGLES);
			for(i=0;i<m_FaceNum;i++)
			{
				glNormal3f(-m_PointNormal[m_Face[i].ii].x,-m_PointNormal[m_Face[i].ii].y,-m_PointNormal[m_Face[i].ii].z);
				glVertex3f(m_Point[m_Face[i].ii].x, m_Point[m_Face[i].ii].y, m_Point[m_Face[i].ii].z);
				glNormal3f(-m_PointNormal[m_Face[i].kk].x,-m_PointNormal[m_Face[i].kk].y,-m_PointNormal[m_Face[i].kk].z);
				glVertex3f(m_Point[m_Face[i].kk].x, m_Point[m_Face[i].kk].y, m_Point[m_Face[i].kk].z);
				glNormal3f(-m_PointNormal[m_Face[i].jj].x,-m_PointNormal[m_Face[i].jj].y,-m_PointNormal[m_Face[i].jj].z);
				glVertex3f(m_Point[m_Face[i].jj].x, m_Point[m_Face[i].jj].y, m_Point[m_Face[i].jj].z);
			}
			glEnd();
		}

		glPopAttrib();
		return 1;
	};

	void DrawWireframe() 
	{
		glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);
		glBegin(GL_LINES);
		for(int i=0;i<m_FaceNum;i++)
		{
			glVertex3f(m_Point[m_Face[i].ii].x, m_Point[m_Face[i].ii].y, m_Point[m_Face[i].ii].z);
			glVertex3f(m_Point[m_Face[i].jj].x, m_Point[m_Face[i].jj].y, m_Point[m_Face[i].jj].z);

			glVertex3f(m_Point[m_Face[i].jj].x, m_Point[m_Face[i].jj].y, m_Point[m_Face[i].jj].z);
			glVertex3f(m_Point[m_Face[i].kk].x, m_Point[m_Face[i].kk].y, m_Point[m_Face[i].kk].z);

			glVertex3f(m_Point[m_Face[i].kk].x, m_Point[m_Face[i].kk].y, m_Point[m_Face[i].kk].z);
			glVertex3f(m_Point[m_Face[i].ii].x, m_Point[m_Face[i].ii].y, m_Point[m_Face[i].ii].z);
		}
		glEnd();
	}

	int DrawPointIndex(vec3d pointColor)
	{
		GLUquadricObj *qobj = 0;
		qobj = gluNewQuadric();
		glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);
		glColor3f(pointColor.x,pointColor.y,pointColor.z);
		for(int i=0;i<m_PointNum;i++)
		{
			glPushMatrix();
			glTranslatef(m_Point[i].x,m_Point[i].y,m_Point[i].z);
			//	gluSphere(qobj,0.1,20,20);
			glPopMatrix();
			char pointNum[20];
			sprintf(pointNum,"%d",i);
			glRasterPos3f(m_Point[i].x,m_Point[i].y,m_Point[i].z);
			glPrint(pointNum);
		}
		glPopAttrib();
		return 1;
	};

	void glPrint(const char *text)
	{
		glPushAttrib(GL_LIST_BIT);                        
		glListBase(1 - 32);                             
		glCallLists(GLsizei(strlen(text)), GL_UNSIGNED_BYTE, text); 
		glPopAttrib();                                                                  
	};

	void DrawVector(vec3d point, double width, vec3d direc, double lengths)
	{
		GLUquadricObj *qobj = 0;
		qobj = gluNewQuadric();
		glLineWidth(width);
		glBegin(GL_LINES);
		glVertex3f(point.x,point.y,point.z);
		glVertex3f(point.x+direc.x*lengths,point.y+direc.y*lengths,point.z+direc.z*lengths);
		glEnd();
		glLineWidth(1);
	};

	int DrawTri(int facenum)
	{
		glPushMatrix();
		glTranslatef(m_FaceNormal[facenum].x*0.01,m_FaceNormal[facenum].y*0.01,m_FaceNormal[facenum].z*-0.01); 
		glBegin(GL_TRIANGLES);
		glVertex3f(m_Point[m_Face[facenum].ii].x, m_Point[m_Face[facenum].ii].y, m_Point[m_Face[facenum].ii].z);
		glVertex3f(m_Point[m_Face[facenum].jj].x, m_Point[m_Face[facenum].jj].y, m_Point[m_Face[facenum].jj].z);
		glVertex3f(m_Point[m_Face[facenum].kk].x, m_Point[m_Face[facenum].kk].y, m_Point[m_Face[facenum].kk].z);
		glEnd();

		glBegin(GL_TRIANGLES);
		glVertex3f(m_Point[m_Face[facenum].ii].x, m_Point[m_Face[facenum].ii].y, m_Point[m_Face[facenum].ii].z);
		glVertex3f(m_Point[m_Face[facenum].kk].x, m_Point[m_Face[facenum].kk].y, m_Point[m_Face[facenum].kk].z);
		glVertex3f(m_Point[m_Face[facenum].jj].x, m_Point[m_Face[facenum].jj].y, m_Point[m_Face[facenum].jj].z);
		glEnd();
		glPopMatrix();
		return 1;
	};

	matrix ComputeRotationMatrix(vec3d axis, double angle)
	{
		matrix ref(3,3);
		double x,y,z;
		x=axis.x;
		y=axis.y;
		z=axis.z;

		ref(0,0)=x*x+(y*y+z*z)*cos(angle);
		ref(1,1)=y*y+(x*x+z*z)*cos(angle);
		ref(2,2)=z*z+(x*x+y*y)*cos(angle);
		ref(0,1)=(1-cos(angle))*x*y+z*sin(angle);
		ref(1,0)=(1-cos(angle))*x*y-z*sin(angle);
		ref(0,2)=(1-cos(angle))*x*z-y*sin(angle);
		ref(2,0)=(1-cos(angle))*z*x+y*sin(angle);
		ref(1,2)=(1-cos(angle))*y*z+x*sin(angle);
		ref(2,1)=(1-cos(angle))*z*y-x*sin(angle);
		return ref;
	};

	void ComputeCenterPoint()
	{
		int i;
		vec3d sum;
		for(i=0;i<m_PointNum;i++)
			sum=sum+m_Point[i];
		sum=sum/m_PointNum;
		m_MidPoint=sum;
	};

	void Rotate(vec3d axis, double angle)
	{
		matrix rot;
		rot=ComputeRotationMatrix(axis, angle);

		//이전 스텝의 point 좌표 저장
		UpdatePreviousPoint();

		for(int i=0;i<m_PointNum;i++)
			m_Point[i]=rot.mulVector(m_Point[i]);

		ComputeCenterPoint();
		UpdateFaceNormalVector();
		UpdatePointNormalVector();
	};

	void Transformation(matrix transform)
	{
		//이전 스텝의 point 좌표 저장
		UpdatePreviousPoint();

		for(int i=0;i<m_PointNum;i++)
		{
			m_Point[i].x=transform(0,0)*m_InitialPoint[i].x+transform(1,0)*m_InitialPoint[i].y+transform(2,0)*m_InitialPoint[i].z+transform(3,0);
			m_Point[i].y=transform(0,1)*m_InitialPoint[i].x+transform(1,1)*m_InitialPoint[i].y+transform(2,1)*m_InitialPoint[i].z+transform(3,1);
			m_Point[i].z=transform(0,2)*m_InitialPoint[i].x+transform(1,2)*m_InitialPoint[i].y+transform(2,2)*m_InitialPoint[i].z+transform(3,2);
		}
		ComputeCenterPoint();
		UpdateFaceNormalVector();
		UpdatePointNormalVector();
	};

	void RotateAtCenter(vec3d axis, double angle)
	{
		matrix rot;
		rot=ComputeRotationMatrix(axis, angle);

		vec3d center=m_MidPoint;

		TranslateObj(center*-1);
		for(int i=0;i<m_PointNum;i++)
			m_Point[i]=rot.mulVector(m_Point[i]);
		TranslateObj(center);

		UpdateFaceNormalVector();
		UpdatePointNormalVector();
	};

	void ScaleObj(float x, float y, float z)
	{
		int i;
		for(i=0;i<m_PointNum;i++)
		{
			m_Point[i].x=m_Point[i].x*x;
			m_Point[i].y=m_Point[i].y*y;
			m_Point[i].z=m_Point[i].z*z;
		}

		ComputeCenterPoint();
		UpdateFaceNormalVector();
		UpdatePointNormalVector();
	};

	void ScaleObj(float scale)
	{
		int i;
		for(i=0;i<m_PointNum;i++)
		{
			m_Point[i]=m_Point[i]*scale;
			m_InitialPoint[i]=m_Point[i];
		}

		ComputeCenterPoint();
		UpdateFaceNormalVector();
		UpdatePointNormalVector();
	};

	void ScaleObjAtCenter(float x, float y, float z)
	{
		int i;
		vec3d center=m_MidPoint;

		TranslateObj(center*-1);
		for(i=0;i<m_PointNum;i++)
		{
			m_Point[i].x=m_Point[i].x*x;
			m_Point[i].y=m_Point[i].y*y;
			m_Point[i].z=m_Point[i].z*z;
			m_InitialPoint[i]=m_Point[i];
		}

		TranslateObj(center);

		UpdateFaceNormalVector();
		UpdatePointNormalVector();
	};

	void ScaleObjAtCenter(float scale)
	{
		int i;
		vec3d center=m_MidPoint;

		TranslateObj(center*-1);
		for(i=0;i<m_PointNum;i++)
		{
			m_Point[i]=m_Point[i]*scale;
			m_InitialPoint[i]=m_Point[i];
		}
		TranslateObj(center);

		UpdateFaceNormalVector();
		UpdatePointNormalVector();
	};

	void TranslateObj(vec3d trans)
	{
		int i;

		//이전 스텝의 point 좌표 저장
		UpdatePreviousPoint();

		for(i=0;i<m_PointNum;i++)
			m_Point[i]=m_Point[i]+trans;

		ComputeCenterPoint();
	};

	void TranslateObj(double x, double y, double z)
	{
		int i;

		//이전 스텝의 point 좌표 저장
		UpdatePreviousPoint();

		for(i=0;i<m_PointNum;i++)
		{
			m_Point[i].x+=x;
			m_Point[i].y+=y;
			m_Point[i].z+=z;
		}

		ComputeCenterPoint();
	};

	void UpdatePreviousPoint()
	{
		int i;
		for(i=0;i<m_PointNum;i++)
			m_PrePoint[i]=m_Point[i];
	}

	vec3d* Point()
	{
		return m_Point;
	};

	vec3d* PrePoint()
	{
		return m_PrePoint;
	};

	vec3d* InitialPoint()
	{
		return m_InitialPoint;
	};

	vec3i* Face()
	{
		return m_Face;
	};

	vec3d* FaceNormal()
	{
		return m_FaceNormal;
	};

	vec3d* PointNormal()
	{
		return m_PointNormal;
	};

	int PointNum()
	{
		return m_PointNum;
	};

	int FaceNum()
	{
		return m_FaceNum;
	};
};

class SModel {
public:
	float m_rad;
	vec3f m_pos;
	vec3d m_vel;

public:
	SModel()
	{};
	virtual ~SModel()
	{};

	int	DrawObject(vec3d color)
	{
		GLUquadricObj *qobj = 0;
		qobj = gluNewQuadric();
		glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);
		glPushMatrix();
		glColor3f(color.x,color.y,color.z);
		//		glColor3f(0,0,1);
		glTranslatef(m_pos[0],m_pos[1],m_pos[2]);
		gluSphere(qobj,m_rad,5,5);
		glPopMatrix();
		glPopAttrib();
		return 1;
	};

	int	Makesphere(float c1,float c2, float c3, float r)
	{
		m_rad=r;
		m_pos[0]=c1;
		m_pos[1]=c2;
		m_pos[2]=c3;
		return 1;
	};

	void TranslateObj(int x, int y, int z)
	{
		m_pos[0]+=x;
		m_pos[1]+=y;
		m_pos[2]+=z;
	};

	void UpdateVel(vec3d n)
	{
		if(m_vel.dot(n)>0)
		{
			vec3d vn=n*m_vel.dot(n);
			vec3d vt=m_vel-vn;
			m_vel=vn*-1+vt;
		}
	};

	void UpdatePos(float dt)
	{
		m_pos[0]=m_pos[0]+m_vel.x*dt;
		m_pos[1]=m_pos[1]+m_vel.y*dt;
		m_pos[2]=m_pos[2]+m_vel.z*dt;
	};
};

#endif






