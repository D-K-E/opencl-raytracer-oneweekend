/*kernel for gradient*/
__kernel void color_gradient(__global float3* output,
        int width, int height){
    const int work_item_id = get_global_id(0);		
    int x_coord = work_item_id % width;	
    int y_coord = work_item_id / width;

    float fx = (float)x_coord / (float)width;  /* convert int in range [0 - width] to float in range [0-1] */
    float fy = (float)y_coord / (float)height; /* convert int in range [0 - height] to float in range [0-1] */
    output[work_item_id] = (float3)(fx, fy, 0);
}
