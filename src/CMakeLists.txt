add_subdirectory(ImageProcessingLib)
add_subdirectory(Pink)
add_subdirectory(SelfOrganizingMapLib)
add_subdirectory(UtilitiesLib)

if(CUDA_FOUND)
    add_subdirectory(CudaLib)
endif()