ifeq ($(USE_SMART_BUILD),yes)
ifneq ($(findstring HAL_USE_SERIAL TRUE,$(HALCONF)),)
PLATFORMSRC += $(CHIBIOS)/os/hal/ports/ADUCM/LLD/UARTv1/hal_serial_lld.c
endif
else
PLATFORMSRC += $(CHIBIOS)/os/hal/ports/ADUCM/LLD/UARTv1/hal_serial_lld.c
endif

PLATFORMINC += $(CHIBIOS)/os/hal/ports/ADUCM/LLD/UARTv1
