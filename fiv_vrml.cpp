#include "fiv_vrml.h"
//#include "us_jpeg.h"              // ???

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

fiv_vrml::fiv_vrml()
{
    m_sub_file_name=new char[200];
    m_sub_file_name_no_dir=new char[80];
    m_sub_file_texture=new char[200]; 
    m_sub_file_texture_no_dir=new char[80];
}

fiv_vrml::~fiv_vrml()
{
    clean();
}

void fiv_vrml::clean()
{
    if(m_sub_file_name)
        delete[] m_sub_file_name;
    if(m_sub_file_name_no_dir)
        delete[] m_sub_file_name_no_dir;
    if(m_sub_file_texture)
        delete[] m_sub_file_texture;
    if(m_sub_file_texture_no_dir)
        delete[] m_sub_file_texture_no_dir;
}


int fiv_vrml::begin_sub_file(char *file_name)
{
    sprintf(m_sub_file_name,file_name);
    m_sub_file=fopen(m_sub_file_name,"wt");
    if(!m_sub_file)
    {
        printf("Sub VRML file %s failed to be opened!\n",m_sub_file_name);
        return(-1);
    }
    else
    {
        //Write header, before objects can be written 
        fprintf(m_sub_file,"#VRML V2.0 utf8\n");
    }
    return(0);
}

int fiv_vrml::end_sub_file()
{

    fclose(m_sub_file);
    return(0);
}

void fiv_vrml::add_triangles(double *triangle_vertices,int *triangle_indices,double *triangle_texture_coords,
                            int nr_triangle_vertices,int nr_triangles,unsigned char *rgb,int w,int h,double transparency,
                            char *sub_file_texture, char *sub_file_texture_nodir)
{
    int i;

    /*Write the texture*/
    sprintf(m_sub_file_texture,sub_file_texture);
    sprintf(m_sub_file_texture_no_dir,sub_file_texture_nodir);
    //us_write_jpeg_rgb(m_sub_file_texture,rgb,w,h);

    fprintf(m_sub_file,"Shape {\n");
    fprintf(m_sub_file,"    appearance Appearance {\n");
    fprintf(m_sub_file,"        material Material {\n");
    fprintf(m_sub_file,"            ambientIntensity 1\n");
    fprintf(m_sub_file,"            diffuseColor 1 1 1\n");
	fprintf(m_sub_file,"            emissiveColor 1 1 1\n");
	fprintf(m_sub_file,"            shininess 0.05\n");
	fprintf(m_sub_file,"            specularColor 0.01 0.01 0.01\n");
	fprintf(m_sub_file,"            transparency %f\n",transparency);
    fprintf(m_sub_file,"        }\n");
    fprintf(m_sub_file,"        texture ImageTexture {\n");
    fprintf(m_sub_file,"            url \"%s\"\n",m_sub_file_texture_no_dir);
    fprintf(m_sub_file,"        }\n");
    fprintf(m_sub_file,"    }\n");
    fprintf(m_sub_file,"    geometry IndexedFaceSet {\n");
    fprintf(m_sub_file,"        ccw FALSE\n");
    fprintf(m_sub_file,"        solid FALSE\n");
    fprintf(m_sub_file,"        creaseAngle 5\n");
    fprintf(m_sub_file,"        coord Coordinate {\n");
    fprintf(m_sub_file,"            point [\n");
    for(i=0;i<3*nr_triangle_vertices;i+=3)
         //  fprintf(m_sub_file,"               %lf %lf %lf,\n",triangle_vertices[i],-triangle_vertices[i+1],triangle_vertices[i+2]);     INVERT y
		fprintf(m_sub_file,"               %lf %lf %lf,\n",triangle_vertices[i],triangle_vertices[i+1],triangle_vertices[i+2]);   
    fprintf(m_sub_file,"            ]\n");
    fprintf(m_sub_file,"        }\n");
    fprintf(m_sub_file,"        coordIndex [\n");
    for(i=0;i<3*nr_triangles;i+=3)
        fprintf(m_sub_file,"           %d,%d,%d,-1,\n",triangle_indices[i],triangle_indices[i+1],triangle_indices[i+2]);
    fprintf(m_sub_file,"        ]\n");
    fprintf(m_sub_file,"        texCoord TextureCoordinate {\n");    
    fprintf(m_sub_file,"            point [\n");
    for(i=0;i<2*nr_triangle_vertices;i+=2)
        fprintf(m_sub_file,"                %lf %lf,\n",triangle_texture_coords[i],1-triangle_texture_coords[i+1]);
    fprintf(m_sub_file,"            ]\n");
    fprintf(m_sub_file,"        }\n");
    fprintf(m_sub_file,"    }\n");
    fprintf(m_sub_file,"}\n");
}

void fiv_vrml::add_triangles(double *triangle_vertices,int *triangle_indices,int nr_triangle_vertices,int nr_triangles,
                            double r,double g,double b,double transparency)
{
    int i;

    fprintf(m_sub_file,"Shape {\n");
    fprintf(m_sub_file,"    appearance Appearance {\n");
    fprintf(m_sub_file,"        material Material {\n");
    fprintf(m_sub_file,"            ambientIntensity 1\n");
    fprintf(m_sub_file,"            diffuseColor %f %f %f\n", r, g, b);
	fprintf(m_sub_file,"            emissiveColor %f %f %f\n", r, g, b);
	fprintf(m_sub_file,"            shininess 0.05\n");
	fprintf(m_sub_file,"            specularColor 0.01 0.01 0.01\n");
	fprintf(m_sub_file,"            transparency %f\n",transparency);
    fprintf(m_sub_file,"        }\n");
    fprintf(m_sub_file,"    }\n");
    fprintf(m_sub_file,"    geometry IndexedFaceSet {\n");
    fprintf(m_sub_file,"        solid FALSE\n");
    fprintf(m_sub_file,"        creaseAngle 5\n");
    fprintf(m_sub_file,"        coord Coordinate {\n");
    fprintf(m_sub_file,"            point [\n");
    for(i=0;i<3*nr_triangle_vertices;i+=3)
        fprintf(m_sub_file,"               %lf %lf %lf,\n",triangle_vertices[i],triangle_vertices[i+1],triangle_vertices[i+2]);
    fprintf(m_sub_file,"            ]\n");
    fprintf(m_sub_file,"        }\n");
    fprintf(m_sub_file,"        coordIndex [\n");
    for(i=0;i<3*nr_triangles;i+=3)
        fprintf(m_sub_file,"           %d,%d,%d,-1,\n",triangle_indices[i],triangle_indices[i+1],triangle_indices[i+2]);
    fprintf(m_sub_file,"        ]\n");
    fprintf(m_sub_file,"    }\n");
    fprintf(m_sub_file,"}\n");
}

void fiv_vrml::add_triangles(double *triangle_vertices,int *triangle_indices,int nr_triangle_vertices,int nr_triangles,
                            double *r,double *g,double *b,double transparency)
{
    int i;

    fprintf(m_sub_file,"Shape {\n");
    fprintf(m_sub_file,"    appearance Appearance {\n");
    fprintf(m_sub_file,"        material Material {\n");
    fprintf(m_sub_file,"            ambientIntensity 1\n");
    fprintf(m_sub_file,"            diffuseColor 1 1 1\n");
	fprintf(m_sub_file,"            emissiveColor 1 1 1\n");
	fprintf(m_sub_file,"            shininess 0.05\n");
	fprintf(m_sub_file,"            specularColor 0.01 0.01 0.01\n");
	fprintf(m_sub_file,"            transparency %f\n",transparency);
    fprintf(m_sub_file,"        }\n");
    fprintf(m_sub_file,"    }\n");
    fprintf(m_sub_file,"    geometry IndexedFaceSet {\n");
    fprintf(m_sub_file,"        solid FALSE\n");
    fprintf(m_sub_file,"        creaseAngle 5\n");
    fprintf(m_sub_file,"        colorPerVertex TRUE \n");

    fprintf(m_sub_file,"        coord Coordinate {\n");
    fprintf(m_sub_file,"            point [\n");
    for(i=0;i<3*nr_triangle_vertices;i+=3)
        fprintf(m_sub_file,"               %lf %lf %lf,\n",triangle_vertices[i],triangle_vertices[i+1],triangle_vertices[i+2]);
    fprintf(m_sub_file,"            ]\n");
    fprintf(m_sub_file,"        }\n");

    fprintf(m_sub_file,"        color Color {\n");
    fprintf(m_sub_file,"            color [\n");
    for(i=0;i<nr_triangle_vertices;i++)
        fprintf(m_sub_file,"           %lf,%lf,%lf,\n",r[i],g[i],b[i]);
    fprintf(m_sub_file,"            ]\n");
    fprintf(m_sub_file,"        }\n");
    
    fprintf(m_sub_file,"        coordIndex [\n");
    for(i=0;i<3*nr_triangles;i+=3)
        fprintf(m_sub_file,"           %d,%d,%d,-1,\n",triangle_indices[i],triangle_indices[i+1],triangle_indices[i+2]);
    fprintf(m_sub_file,"        ]\n");



    fprintf(m_sub_file,"    }\n");
    fprintf(m_sub_file,"}\n");
}

void fiv_vrml::add_points(double *points_vertices,double *points_colors,int nr_points,
                         bool uniform_color,double r,double g,double b)
{
    int i;
    fprintf(m_sub_file,"Shape {\n");
    fprintf(m_sub_file,"    geometry PointSet {\n");
    fprintf(m_sub_file,"        color Color {\n");
    fprintf(m_sub_file,"            color [\n");
    if(uniform_color)
        for(i=0;i<nr_points;i++)
            fprintf(m_sub_file,"                %f %f %f,\n",r,g,b);
    else
        for(i=0;i<3*nr_points;i+=3)
            fprintf(m_sub_file,"                %f %f %f,\n",points_colors[i],points_colors[i+1],points_colors[i+2]);
    fprintf(m_sub_file,"            ]\n");
    fprintf(m_sub_file,"        }\n");
    fprintf(m_sub_file,"        coord Coordinate {\n");
    fprintf(m_sub_file,"            point [\n");
    for(i=0;i<3*nr_points;i+=3)
        fprintf(m_sub_file,"                %lf %lf %lf,\n",points_vertices[i],points_vertices[i+1],points_vertices[i+2]);
    fprintf(m_sub_file,"            ]\n");
    fprintf(m_sub_file,"        }\n");
    fprintf(m_sub_file,"    }\n");
    fprintf(m_sub_file,"}\n");
}

void fiv_vrml::add_lines(double *line_vertices,double *line_colors,int nr_lines)
{
    int i;
    fprintf(m_sub_file,"Shape {\n");
    fprintf(m_sub_file,"    geometry IndexedLineSet {\n");
    fprintf(m_sub_file,"        color Color {\n");
    fprintf(m_sub_file,"            color [\n");
    for(i=0;i<3*2*nr_lines;i+=3)
        fprintf(m_sub_file,"                %lf %lf %lf,\n",line_colors[i],line_colors[i+1],line_colors[i+2]);
    fprintf(m_sub_file,"            ]\n");
    fprintf(m_sub_file,"        }\n");
    fprintf(m_sub_file,"        coord Coordinate {\n");
    fprintf(m_sub_file,"            point [\n");
    for(i=0;i<3*2*nr_lines;i+=3)
        fprintf(m_sub_file,"                %lf %lf %lf,\n",line_vertices[i],line_vertices[i+1],line_vertices[i+2]);
    fprintf(m_sub_file,"            ]\n");
    fprintf(m_sub_file,"        }\n");
    fprintf(m_sub_file,"        coordIndex [\n");
    for(i=0;i<2*nr_lines;i+=2)
        fprintf(m_sub_file,"            %d,%d,-1,\n",i,i+1);
    fprintf(m_sub_file,"        ]\n");
    fprintf(m_sub_file,"    }\n");
    fprintf(m_sub_file,"}\n");
}

