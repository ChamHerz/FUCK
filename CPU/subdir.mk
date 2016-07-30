################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../cliente.c \
../cpu.c \
../direccion.c \
../main.c \
../pcb.c \
../primitivas.c 

OBJS += \
./cliente.o \
./cpu.o \
./direccion.o \
./main.o \
./pcb.o \
./primitivas.o 

C_DEPS += \
./cliente.d \
./cpu.d \
./direccion.d \
./main.d \
./pcb.d \
./primitivas.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


