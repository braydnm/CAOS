// #include "../kernel.h"

// #define INDEX_VAL(x) (x / 32)
// #define OFFSET_VAL(x) (x % 32)

// static uint32_t num_frames = 0;
// static uint32_t* frames = 0;
// page_dir_t* kernel_page_dir = 0;

// void set_frames(int n){
//     num_frames = n;
//     if (frames){
//         kprintf("Cannot set %d frames, table is already intialized\n");
//         while(true);
//     }

//     // we allocate n / 8 bits as we need n / 32 uint32_t but
//     // each uint32_t is 4 bytes
//     frames = (uint32_t*)kmalloc(n / 8);
//     memset(frames, 0, n / 4);

//     uintptr_t p;
//     kernel_page_dir = kvmalloc_p(sizeof(page_dir_t), &p);
//     kprintf("Kernel page dir is located at 0x%x\n", kernel_page_dir);
//     memset(kernel_page_dir, 0, sizeof(page_dir_t));
// }

// uint32_t get_num_frames(){
//     return num_frames;
// }

// void set_frame(uintptr_t a){
//     // each page corresponds to 4096 bytes of memory
//     // or 0x1000 bytes
//     uintptr_t tmp = a /0x1000;
//     int index = INDEX_VAL(tmp);
//     int offset = OFFSET_VAL(tmp);
//     frames[index] |= (1 << offset);
// }

// void clear_frame(uintptr_t a){
//     uintptr_t tmp = a /0x1000;
//     int index = INDEX_VAL(tmp);
//     int offset = OFFSET_VAL(tmp);
//     frames[index] &= ~(1 << offset);
// }

// bool test_frame(uintptr_t a){
//     uintptr_t tmp = a / 0x1000;
//     int index = INDEX_VAL(tmp);
//     int offset = OFFSET_VAL(tmp);
//     return frames[index] & (1 << offset);
// }

// uint32_t find_first_frame_empty_n(int n){
//     for (int i = 0; i < num_frames * 0x1000; i += 0x1000){
//         int b = 0;
//         for (int j = 0; j < n; j++)
//             if (test_frame(i + 0x1000 * j))
//                 b = j + 1;
        
//         if (!b)
//             return i / 0x1000;
//     }

//     return 0xFFFFFFFF;
// }

