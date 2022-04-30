#include <stdint.h>
#include <math.h>

// Intrinsic auxiliary library
#include "../../isa_intrinsic_lib/isa_intrinsic_aux_lib.h"
#include "../../isa_intrinsic_lib/core_class_wrapper.h"

#define MAIN_MEMORY_SIZE   1024*1024*1024 // in bytes -> 512 MB // [0 ~ 1073741824]
#define RF_SIZE  1024 // in entries, FIXED! each 24-bit
#define LM_SIZE  8192 // number of entries, each 16-bit wide

#ifndef NUM_CLUSTERS
#define NUM_CLUSTERS  1 // number of clusters
#endif
#ifndef NUM_VU_PER_CLUSTER
#define NUM_VU_PER_CLUSTER  1 // vector units per cluster
#endif
#define NUM_VECTORLANES  2 // per unit

#ifndef CNN_YOLO_LITE_DEFINES_H
#define CNN_YOLO_LITE_DEFINES_H


constexpr int reference_datatype = 32;
constexpr bool reference_datatype_float = false;



#endif //CNN_YOLO_LITE_DEFINES_H



//----------------------------------------------------------------------------------
//----------------------------------Main--------------------------------------------
//----------------------------------------------------------------------------------
int main(int argc, char *argv[]) {
    // initialize simulator environment
    sim_init(main, argc, argv, MAIN_MEMORY_SIZE, LM_SIZE, RF_SIZE, NUM_CLUSTERS, NUM_VU_PER_CLUSTER, NUM_VECTORLANES);
    sim_printf("Clusters: %i, units: %i, lanes: %i", NUM_CLUSTERS, NUM_VU_PER_CLUSTER, NUM_VECTORLANES);
    // execute each on ALL vector units
    vpro_set_idmask(0xFFFFFFFF);

   // const int16_t kernel_outline[9] = {-1, -1, -1, -1, 8, -1, -1, -1, -1};
   // const int16_t kernel_blur[9] = {1, 2, 1, 2, 4, 2, 1, 2, 1};
   // const int16_t kernel_sharpen[9] = {0, -1, 0, -1, 5, -1, 0, -1, 0};
   // const int16_t kernel_laplace[9] = {0, 1, 0, 1, -4, 1, 0, 1, 0};
   // const int16_t kernel_sobel_x[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
   // const int16_t kernel_sobel_y[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
  // const int16_t G[25] = {0.99951871 ,0.99784303 ,0.99644375 ,0.99784303 ,0.99951871 ,0.99784303
 //,0.99033311 ,0.984062 ,0.99033311 ,0.99784303 ,0.99644375 ,0.984062
// 0.97372268 ,0.984062 ,0.99644375 ,0.99784303 ,0.99033311 0.984062
// 0.99033311 ,0.99784303 ,0.99951871 ,0.99784303 ,0.99644375 ,0.99784303
// ,0.99951871};



 
    
    // ************************** YOUR CODE **************************** //

//---------------------------------------------label2D finder------------------------------------------------
if (bool label2D_to_neibor=true) //to activate label2D neibor finder
{
int upload_legnth=16;
int S=5;
bool pad_flags[4];
pad_flags[CommandDMA::PAD::TOP] = true;
pad_flags[CommandDMA::PAD::RIGHT] = true;
pad_flags[CommandDMA::PAD::BOTTOM] = true;
pad_flags[CommandDMA::PAD::LEFT] = true;
//--------------------------------------------------

dma_set_pad_widths(2,0,0,2);
dma_set_pad_value(0);
dma_ext2D_to_loc1D(0,0x0,0x0,1026-(upload_legnth+4)+1,upload_legnth+4,upload_legnth+4,pad_flags);
dma_wait_to_finish(0xFFFF);        
for (int z=0; z<16;z=z+2)
    {
        int offset_fixer=0;
            for (int y=0+z; y<2+z;y++)
                    {
                    for (int x=0; x<upload_legnth;x++)
                        {
                    
                        __builtin_vpro_instruction_word(LS, NONBLOCKING, IS_CHAIN, FUNC_LOAD, NO_FLAG_UPDATE,
                        DST_ADDR(0, 0, 0), SRC1_ADDR(x, 1, upload_legnth+4), SRC2_IMM(y*(upload_legnth+4)), S-1, S-1);
                        __builtin_vpro_instruction_word(L0, NONBLOCKING, NO_CHAIN, FUNC_ADD, NO_FLAG_UPDATE,
                        DST_ADDR(S*S*offset_fixer, 1, S*S), SRC1_LS, SRC2_IMM(0), S*S-1, 0);
                        vpro_wait_busy(0xFFFF, 0xFFFF);
                        offset_fixer++;   

                        }
                            
                    }
            

                __builtin_vpro_instruction_word(L0, NONBLOCKING, IS_CHAIN, FUNC_ADD, NO_FLAG_UPDATE,
                    DST_ADDR(0, 1, S*S), SRC1_ADDR(0, 1, S*S), SRC2_IMM(0), S*S-1, 2*upload_legnth-1);
                    
                __builtin_vpro_instruction_word(LS, NONBLOCKING, NO_CHAIN, FUNC_STORE, NO_FLAG_UPDATE,
                DST_ADDR(400, 1, S*S), SRC1_CHAINING(0), SRC2_IMM(z*upload_legnth*S*S), S*S-1, 2*upload_legnth-1);   
                vpro_wait_busy(0xFFFF, 0xFFFF); 
}                
for (int y=0;y<upload_legnth;y++)   
{  
dma_loc1D_to_ext2D(0,65536*2+(y*1024*25*2),400+y*upload_legnth*25,1,25,16);
}    
dma_wait_to_finish(0xFFFF);

//--------------------------------------------------
dma_set_pad_widths(2,2,0,0);
dma_set_pad_value(0);
dma_ext2D_to_loc1D(0,1006*2,0,1026-(upload_legnth+4)+1,upload_legnth+4,upload_legnth+4,pad_flags);
dma_wait_to_finish(0xFFFF);       
for (int z=0; z<16;z=z+2)
    {
        int offset_fixer=0;
            for (int y=0+z; y<2+z;y++)
                    {
                    for (int x=0; x<upload_legnth;x++)
                        {
                    
                        __builtin_vpro_instruction_word(LS, NONBLOCKING, IS_CHAIN, FUNC_LOAD, NO_FLAG_UPDATE,
                        DST_ADDR(0, 0, 0), SRC1_ADDR(x, 1, upload_legnth+4), SRC2_IMM(y*(upload_legnth+4)), S-1, S-1);
                        __builtin_vpro_instruction_word(L0, NONBLOCKING, NO_CHAIN, FUNC_ADD, NO_FLAG_UPDATE,
                        DST_ADDR(S*S*offset_fixer, 1, S*S), SRC1_LS, SRC2_IMM(0), S*S-1, 0);
                        vpro_wait_busy(0xFFFF, 0xFFFF);
                        offset_fixer++;   

                        }
                            
                    }
            

                __builtin_vpro_instruction_word(L0, NONBLOCKING, IS_CHAIN, FUNC_ADD, NO_FLAG_UPDATE,
                    DST_ADDR(0, 1, S*S), SRC1_ADDR(0, 1, S*S), SRC2_IMM(0), S*S-1, 2*upload_legnth-1);
                    
                __builtin_vpro_instruction_word(LS, NONBLOCKING, NO_CHAIN, FUNC_STORE, NO_FLAG_UPDATE,
                DST_ADDR(400, 1, S*S), SRC1_CHAINING(0), SRC2_IMM(z*upload_legnth*S*S), S*S-1, 2*upload_legnth-1);   
                vpro_wait_busy(0xFFFF, 0xFFFF); 
}                
    for (int y=0;y<upload_legnth;y++)   
    {  
    dma_loc1D_to_ext2D(0,131072+((y*1024*25)+1008*25)*2,400+y*upload_legnth*25,1,25,16);
    }    
    dma_wait_to_finish(0xFFFF); 

//--------------------------------------------------------


//for the middel part without padding
for (int n=1;n<64-1;n++)
{   dma_set_pad_widths(2,0,0,0);
    dma_set_pad_value(0);
    dma_ext2D_to_loc1D(0,0+(upload_legnth*n-2)*2,0,1024-(upload_legnth+4)+1,upload_legnth+4,upload_legnth+4,pad_flags);
    dma_wait_to_finish(0xFFFF);
        for (int z=0; z<16;z=z+2)
            {
                int offset_fixer=0;
                    for (int y=0+z; y<2+z;y++)
                            {
                            for (int x=0; x<upload_legnth;x++)
                                {
                            
                                __builtin_vpro_instruction_word(LS, NONBLOCKING, IS_CHAIN, FUNC_LOAD, NO_FLAG_UPDATE,
                                DST_ADDR(0, 0, 0), SRC1_ADDR(x, 1, upload_legnth+4), SRC2_IMM(y*(upload_legnth+4)), S-1, S-1);
                                __builtin_vpro_instruction_word(L0, NONBLOCKING, NO_CHAIN, FUNC_ADD, NO_FLAG_UPDATE,
                                DST_ADDR(S*S*offset_fixer, 1, S*S), SRC1_LS, SRC2_IMM(0), S*S-1, 0);
                                vpro_wait_busy(0xFFFF, 0xFFFF);
                                offset_fixer++;   

                                }
                                    
                            }
                    

                        __builtin_vpro_instruction_word(L0, NONBLOCKING, IS_CHAIN, FUNC_ADD, NO_FLAG_UPDATE,
                            DST_ADDR(0, 1, S*S), SRC1_ADDR(0, 1, S*S), SRC2_IMM(0), S*S-1, 2*upload_legnth-1);
                            
                        __builtin_vpro_instruction_word(LS, NONBLOCKING, NO_CHAIN, FUNC_STORE, NO_FLAG_UPDATE,
                        DST_ADDR(400, 1, S*S), SRC1_CHAINING(0), SRC2_IMM(z*upload_legnth*S*S), S*S-1, 2*upload_legnth-1);   
                        vpro_wait_busy(0xFFFF, 0xFFFF); 
        }                
    for (int y=0;y<upload_legnth;y++)   
        {  
        dma_loc1D_to_ext2D(0,131072+(y*1024*25)*2+(upload_legnth*n*25)*2,400+y*upload_legnth*25,1,25,16);
        }    
        dma_wait_to_finish(0xFFFF);     
}

//------------------------------------left side padding

for (int n_outer=1;n_outer<3;n_outer++)
{      
    dma_set_pad_widths(0,0,0,2);
    dma_set_pad_value(0);   
    dma_ext2D_to_loc1D(0,((1024*upload_legnth*n_outer)-2*1024)*2,0,1026-(upload_legnth+4)+1,upload_legnth+4,upload_legnth+4,pad_flags);
    dma_wait_to_finish(0xFFFF);
        for (int z=0; z<16;z=z+2)
            {
                int offset_fixer=0;
                    for (int y=0+z; y<2+z;y++)
                            {
                            for (int x=0; x<upload_legnth;x++)
                                {
                            
                                __builtin_vpro_instruction_word(LS, NONBLOCKING, IS_CHAIN, FUNC_LOAD, NO_FLAG_UPDATE,
                                DST_ADDR(0, 0, 0), SRC1_ADDR(x, 1, upload_legnth+4), SRC2_IMM(y*(upload_legnth+4)), S-1, S-1);
                                __builtin_vpro_instruction_word(L0, NONBLOCKING, NO_CHAIN, FUNC_ADD, NO_FLAG_UPDATE,
                                DST_ADDR(S*S*offset_fixer, 1, S*S), SRC1_LS, SRC2_IMM(0), S*S-1, 0);
                                vpro_wait_busy(0xFFFF, 0xFFFF);
                                offset_fixer++;   

                                }
                                    
                            }
                    

                        __builtin_vpro_instruction_word(L0, NONBLOCKING, IS_CHAIN, FUNC_ADD, NO_FLAG_UPDATE,
                            DST_ADDR(0, 1, S*S), SRC1_ADDR(0, 1, S*S), SRC2_IMM(0), S*S-1, 2*upload_legnth-1);
                            
                        __builtin_vpro_instruction_word(LS, NONBLOCKING, NO_CHAIN, FUNC_STORE, NO_FLAG_UPDATE,
                        DST_ADDR(400, 1, S*S), SRC1_CHAINING(0), SRC2_IMM(z*upload_legnth*S*S), S*S-1, 2*upload_legnth-1);   
                        vpro_wait_busy(0xFFFF, 0xFFFF); 
        }                
            for (int y=0;y<upload_legnth;y++)   
                {  
                dma_loc1D_to_ext2D(0,131072+((y*1024*25)+1024*upload_legnth*25*n_outer)*2,400+y*upload_legnth*25,1,25,16);
                }    
                dma_wait_to_finish(0xFFFF);     

}

//-----------------------------------right side padding
dma_set_pad_widths(0,2,0,0);
dma_set_pad_value(0);
for (int n_outer=1;n_outer<3;n_outer++)
{   
        int n=63;
        
        dma_ext2D_to_loc1D(0,0+((upload_legnth*n-2)+(1024*upload_legnth*n_outer)-2*1024)*2,0x0,1026-(upload_legnth+4)+1,upload_legnth+4,upload_legnth+4,pad_flags);
        dma_wait_to_finish(0xFFFF);
            for (int z=0; z<16;z=z+2)
                {
                    int offset_fixer=0;
                        for (int y=0+z; y<2+z;y++)
                                {
                                for (int x=0; x<upload_legnth;x++)
                                    {
                                
                                    __builtin_vpro_instruction_word(LS, NONBLOCKING, IS_CHAIN, FUNC_LOAD, NO_FLAG_UPDATE,
                                    DST_ADDR(0, 0, 0), SRC1_ADDR(x, 1, upload_legnth+4), SRC2_IMM(y*(upload_legnth+4)), S-1, S-1);
                                    __builtin_vpro_instruction_word(L0, NONBLOCKING, NO_CHAIN, FUNC_ADD, NO_FLAG_UPDATE,
                                    DST_ADDR(S*S*offset_fixer, 1, S*S), SRC1_LS, SRC2_IMM(0), S*S-1, 0);
                                    vpro_wait_busy(0xFFFF, 0xFFFF);
                                    offset_fixer++;   

                                    }
                                        
                                }
                        

                            __builtin_vpro_instruction_word(L0, NONBLOCKING, IS_CHAIN, FUNC_ADD, NO_FLAG_UPDATE,
                                DST_ADDR(0, 1, S*S), SRC1_ADDR(0, 1, S*S), SRC2_IMM(0), S*S-1, 2*upload_legnth-1);
                                
                            __builtin_vpro_instruction_word(LS, NONBLOCKING, NO_CHAIN, FUNC_STORE, NO_FLAG_UPDATE,
                            DST_ADDR(400, 1, S*S), SRC1_CHAINING(0), SRC2_IMM(z*upload_legnth*S*S), S*S-1, 2*upload_legnth-1);   
                            vpro_wait_busy(0xFFFF, 0xFFFF); 
            }                
                for (int y=0;y<upload_legnth;y++)   
                    {  
                    dma_loc1D_to_ext2D(0,131072+((y*1024*25)+(upload_legnth*n*25)+1024*upload_legnth*25*n_outer)*2,400+y*upload_legnth*25,1,25,16);
                    }    
                    dma_wait_to_finish(0xFFFF);     
    
}

//-----------------------------------without padding for inner section
dma_set_pad_widths(0,0,0,0);
dma_set_pad_value(0);
for (int n_outer=1;n_outer<3;n_outer++)
{   
    for (int n=1;n<64-1;n++)
    {
        
        dma_ext2D_to_loc1D(0,0+((upload_legnth*n-2)+(1024*upload_legnth*n_outer)-2*1024)*2,0x0,1024-(upload_legnth+4)+1,upload_legnth+4,upload_legnth+4);
        dma_wait_to_finish(0xFFFF);
            for (int z=0; z<16;z=z+2)
                {
                    int offset_fixer=0;
                        for (int y=0+z; y<2+z;y++)
                                {
                                for (int x=0; x<upload_legnth;x++)
                                    {
                                
                                    __builtin_vpro_instruction_word(LS, NONBLOCKING, IS_CHAIN, FUNC_LOAD, NO_FLAG_UPDATE,
                                    DST_ADDR(0, 0, 0), SRC1_ADDR(x, 1, upload_legnth+4), SRC2_IMM(y*(upload_legnth+4)), S-1, S-1);
                                    __builtin_vpro_instruction_word(L0, NONBLOCKING, NO_CHAIN, FUNC_ADD, NO_FLAG_UPDATE,
                                    DST_ADDR(S*S*offset_fixer, 1, S*S), SRC1_LS, SRC2_IMM(0), S*S-1, 0);
                                    vpro_wait_busy(0xFFFF, 0xFFFF);
                                    offset_fixer++;   

                                    }
                                        
                                }
                        

                            __builtin_vpro_instruction_word(L0, NONBLOCKING, IS_CHAIN, FUNC_ADD, NO_FLAG_UPDATE,
                                DST_ADDR(0, 1, S*S), SRC1_ADDR(0, 1, S*S), SRC2_IMM(0), S*S-1, 2*upload_legnth-1);
                                
                            __builtin_vpro_instruction_word(LS, NONBLOCKING, NO_CHAIN, FUNC_STORE, NO_FLAG_UPDATE,
                            DST_ADDR(400, 1, S*S), SRC1_CHAINING(0), SRC2_IMM(z*upload_legnth*S*S), S*S-1, 2*upload_legnth-1);   
                            vpro_wait_busy(0xFFFF, 0xFFFF); 
            }                
                for (int y=0;y<upload_legnth;y++)   
                    {  
                    dma_loc1D_to_ext2D(0,131072+((y*1024*25)+(upload_legnth*n*25)+1024*upload_legnth*25*n_outer)*2,400+y*upload_legnth*25,1,25,16);
                    }    
                    dma_wait_to_finish(0xFFFF);     
    }
}
//-----------------------------------------------------right under part

dma_set_pad_widths(0,2,2,0);
dma_set_pad_value(0);
dma_ext2D_to_loc1D(0,(1024*3*16-2*1024+1006)*2,0x0,1026-(upload_legnth+4)+1,upload_legnth+4,upload_legnth+4,pad_flags);
dma_wait_to_finish(0xFFFF);       
for (int z=0; z<16;z=z+2)
    {
        int offset_fixer=0;
            for (int y=0+z; y<2+z;y++)
                    {
                    for (int x=0; x<upload_legnth;x++)
                        {
                    
                        __builtin_vpro_instruction_word(LS, NONBLOCKING, IS_CHAIN, FUNC_LOAD, NO_FLAG_UPDATE,
                        DST_ADDR(0, 0, 0), SRC1_ADDR(x, 1, upload_legnth+2+2), SRC2_IMM(y*(upload_legnth+2+2)), S-1, S-1);
                        __builtin_vpro_instruction_word(L0, NONBLOCKING, NO_CHAIN, FUNC_ADD, NO_FLAG_UPDATE,
                        DST_ADDR(S*S*offset_fixer, 1, S*S), SRC1_LS, SRC2_IMM(0), S*S-1, 0);
                        vpro_wait_busy(0xFFFF, 0xFFFF);
                        offset_fixer++;   

                        }
                            
                    }
            

                __builtin_vpro_instruction_word(L0, NONBLOCKING, IS_CHAIN, FUNC_ADD, NO_FLAG_UPDATE,
                    DST_ADDR(0, 1, S*S), SRC1_ADDR(0, 1, S*S), SRC2_IMM(0), S*S-1, 2*upload_legnth-1);
                    
                __builtin_vpro_instruction_word(LS, NONBLOCKING, NO_CHAIN, FUNC_STORE, NO_FLAG_UPDATE,
                DST_ADDR(400, 1, S*S), SRC1_CHAINING(0), SRC2_IMM(z*upload_legnth*S*S), S*S-1, 2*upload_legnth-1);   
                vpro_wait_busy(0xFFFF, 0xFFFF); 
}                
for (int y=0;y<upload_legnth;y++)   
{  
dma_loc1D_to_ext2D(0,131072+((y*1024)+16*3*1024+1008)*25*2,400+y*upload_legnth*25,1,25,16);
}    
dma_wait_to_finish(0xFFFF);

//-----------------------------------------------------left under part

dma_set_pad_widths(0,0,2,2);
dma_set_pad_value(0);
dma_ext2D_to_loc1D(0,(1024*3*16-2*1024)*2,0x0,1026-(upload_legnth+4)+1,upload_legnth+4,upload_legnth+4,pad_flags);
dma_wait_to_finish(0xFFFF);       
for (int z=0; z<16;z=z+2)
    {
        int offset_fixer=0;
            for (int y=0+z; y<2+z;y++)
                    {
                    for (int x=0; x<upload_legnth;x++)
                        {
                    
                        __builtin_vpro_instruction_word(LS, NONBLOCKING, IS_CHAIN, FUNC_LOAD, NO_FLAG_UPDATE,
                        DST_ADDR(0, 0, 0), SRC1_ADDR(x, 1, upload_legnth+2+2), SRC2_IMM(y*(upload_legnth+2+2)), S-1, S-1);
                        __builtin_vpro_instruction_word(L0, NONBLOCKING, NO_CHAIN, FUNC_ADD, NO_FLAG_UPDATE,
                        DST_ADDR(S*S*offset_fixer, 1, S*S), SRC1_LS, SRC2_IMM(0), S*S-1, 0);
                        vpro_wait_busy(0xFFFF, 0xFFFF);
                        offset_fixer++;   

                        }
                            
                    }
            

                __builtin_vpro_instruction_word(L0, NONBLOCKING, IS_CHAIN, FUNC_ADD, NO_FLAG_UPDATE,
                    DST_ADDR(0, 1, S*S), SRC1_ADDR(0, 1, S*S), SRC2_IMM(0), S*S-1, 2*upload_legnth-1);
                    
                __builtin_vpro_instruction_word(LS, NONBLOCKING, NO_CHAIN, FUNC_STORE, NO_FLAG_UPDATE,
                DST_ADDR(400, 1, S*S), SRC1_CHAINING(0), SRC2_IMM(z*upload_legnth*S*S), S*S-1, 2*upload_legnth-1);   
                vpro_wait_busy(0xFFFF, 0xFFFF); 
}                
for (int y=0;y<upload_legnth;y++)   
{  
dma_loc1D_to_ext2D(0,131072+((y*1024)+16*3*1024)*25*2,400+y*upload_legnth*25,1,25,16);
}    
dma_wait_to_finish(0xFFFF); 

//----------------------------------------------under middle part

dma_set_pad_widths(0,0,2,0);
dma_set_pad_value(0);
   
    for (int n=1;n<64-1;n++)
    {
        
        dma_ext2D_to_loc1D(0,(3*upload_legnth*1024+(upload_legnth*n-2)-2*1024)*2,0x0,1024-(upload_legnth+4)+1,upload_legnth+4,upload_legnth+4,pad_flags);
        dma_wait_to_finish(0xFFFF);
            for (int z=0; z<16;z=z+2)
                {
                    int offset_fixer=0;
                        for (int y=0+z; y<2+z;y++)
                                {
                                for (int x=0; x<upload_legnth;x++)
                                    {
                                
                                    __builtin_vpro_instruction_word(LS, NONBLOCKING, IS_CHAIN, FUNC_LOAD, NO_FLAG_UPDATE,
                                    DST_ADDR(0, 0, 0), SRC1_ADDR(x, 1, upload_legnth+4), SRC2_IMM(y*(upload_legnth+4)), S-1, S-1);
                                    __builtin_vpro_instruction_word(L0, NONBLOCKING, NO_CHAIN, FUNC_ADD, NO_FLAG_UPDATE,
                                    DST_ADDR(S*S*offset_fixer, 1, S*S), SRC1_LS, SRC2_IMM(0), S*S-1, 0);
                                    vpro_wait_busy(0xFFFF, 0xFFFF);
                                    offset_fixer++;   

                                    }
                                        
                                }
                        

                            __builtin_vpro_instruction_word(L0, NONBLOCKING, IS_CHAIN, FUNC_ADD, NO_FLAG_UPDATE,
                                DST_ADDR(0, 1, S*S), SRC1_ADDR(0, 1, S*S), SRC2_IMM(0), S*S-1, 2*upload_legnth-1);
                                
                            __builtin_vpro_instruction_word(LS, NONBLOCKING, NO_CHAIN, FUNC_STORE, NO_FLAG_UPDATE,
                            DST_ADDR(400, 1, S*S), SRC1_CHAINING(0), SRC2_IMM(z*upload_legnth*S*S), S*S-1, 2*upload_legnth-1);   
                            vpro_wait_busy(0xFFFF, 0xFFFF); 
            }                
                for (int y=0;y<upload_legnth;y++)   
                    {  
                    dma_loc1D_to_ext2D(0,131072+((y*1024)+upload_legnth*n+3*upload_legnth*1024)*25*2,400+y*upload_legnth*25,1,25,16);
                    }    
                    dma_wait_to_finish(0xFFFF);     
    }

}
//-----------------------------------------------------------------------------------------------------------

//

sim_stop();
    return 0;
}

