idf_component_register(SRCS "gt911.c"
                     INCLUDE_DIRS "include" 
                     INCLUDE_DIRS ${INCLUDES}
   					 REQUIRES driver
   					 REQUIRES lvgl )

