################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ansisop.c \
../consola.c \
../consolas.c \
../cpu.c \
../cpus.c \
../file.c \
../io.c \
../ios.c \
../log.c \
../main.c \
../nucleo.c \
../pcb.c \
../pcbs.c \
../semaforo.c \
../semaforos.c \
../servidor.c \
../util.c \
../variable.c \
../variables.c 

OBJS += \
./ansisop.o \
./consola.o \
./consolas.o \
./cpu.o \
./cpus.o \
./file.o \
./io.o \
./ios.o \
./log.o \
./main.o \
./nucleo.o \
./pcb.o \
./pcbs.o \
./semaforo.o \
./semaforos.o \
./servidor.o \
./util.o \
./variable.o \
./variables.o 

C_DEPS += \
./ansisop.d \
./consola.d \
./consolas.d \
./cpu.d \
./cpus.d \
./file.d \
./io.d \
./ios.d \
./log.d \
./main.d \
./nucleo.d \
./pcb.d \
./pcbs.d \
./semaforo.d \
./semaforos.d \
./servidor.d \
./util.d \
./variable.d \
./variables.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


