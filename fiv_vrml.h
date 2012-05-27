#ifndef FIV_VRML_H
#define FIV_VRML_H

#include <stdio.h>

class fiv_vrml
{
public:
    fiv_vrml();
    ~fiv_vrml();

    /*Releases file name buffers.*/
    void clean();

    /*Begin adding to a new sub file. Returns 0 if ok, otherwise error code.*/
    int begin_sub_file(char *file_name);

    /*End adding to sub file and add sub file into composite file. Returns 0 if ok, otherwise error code.*/
    int end_sub_file();   

    /*Adds triangles and image texture.*/
    void add_triangles(double *triangle_vertices,int *triangle_indices,double *triangle_texture_coords,
                       int nr_triangle_vertices,int nr_triangles,unsigned char *rgb,int w,int h,double transparency,
                       char *sub_file_texture, char *sub_file_texture_nodir);
    
    /*Adds colored triangles.*/
    void add_triangles(double *triangle_vertices,int *triangle_indices,int nr_triangle_vertices,int nr_triangles,
                       double r,double g,double b,double transparency);

    /*Adds indexed colored triangles.*/
    void add_triangles(double *triangle_vertices,int *triangle_indices,int nr_triangle_vertices,int nr_triangles,
                       double *r,double *g,double *b,double transparency);

    /*Adds points with color.*/
    void add_points(double *points_vertices,double *points_colors,int nr_points,
                    bool uniform_color=false,double r=1.0,double g=1.0,double b=1.0);

    /*Adds lines with color.*/
    void add_lines(double *line_vertices,double *line_colors,int nr_lines);

private:
    char *m_sub_file_name,*m_sub_file_name_no_dir;
    char *m_sub_file_texture,*m_sub_file_texture_no_dir;
    FILE *m_composite_file,*m_sub_file;
};

#endif