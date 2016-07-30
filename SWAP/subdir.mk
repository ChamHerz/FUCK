################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ansisop.c \
../bitmap.c \
../direccion.c \
../file.c \
../log.c \
../main.c \
../programa.c \
../programas.c \
../servidor.c \
../swap.c 

OBJS += \
./ansisop.o \
./bitmap.o \
./direccion.o \
./file.o \
./log.o \
./main.o \
./programa.o \
./programas.o \
./servidor.o \
./swap.o 

C_DEPS += \
./ansisop.d \
./bitmap.d \
./direccion.d \
./file.d \
./log.d \
./main.d \
./programa.d \
./programas.d \
./servidor.d \
./swap.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


