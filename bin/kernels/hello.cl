/* simple kernel */
__kernel void parallel_add(
        __global float* x, __global float* y,
        __global float* z){
    const int i = get_global_id(0); /* get global */
    /*get a unique number identifying the work item in the global pool*/
    z[i] = y[i] + x[i]; /*add two arrays*/
}
