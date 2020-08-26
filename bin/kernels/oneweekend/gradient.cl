// gradient kernel

__kernel void ray_color(__global float3* out, 
                        int imwidth,
                        int imheight){
    const int gid = get_global_id(0);
    int xc = gid % imwidth;
    int yc = gid / imwidth;

    float fx = (float)xc / (float)imwidth;
    float fy = (float)yc / (float)imheight;
    out[gid] = (float3)(fx, fy, 0.0);
}
