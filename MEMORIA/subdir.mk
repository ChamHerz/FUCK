################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ansisop.c \
../cpu.c \
../cpus.c \
../direccion.c \
../interprete.c \
../log.c \
../main.c \
../marco.c \
../marcos.c \
../memoria.c \
../programa.c \
../programas.c \
../servidor.c \
../tlb.c \
../tlbs.c \
../util.c 

OBJS += \
./ansisop.o \
./cpu.o \
./cpus.o \
./direccion.o \
./interprete.o \
./log.o \
./main.o \
./marco.o \
./marcos.o \
./memoria.o \
./programa.o \
./programas.o \
./servidor.o \
./tlb.o \
./tlbs.o \
./util.o 

C_DEPS += \
./ansisop.d \
./cpu.d \
./cpus.d \
./direccion.d \
./interprete.d \
./log.d \
./main.d \
./marco.d \
./marcos.d \
./memoria.d \
./programa.d \
./programas.d \
./servidor.d \
./tlb.d \
./tlbs.d \
./util.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


