#include <defs.h>
#include <idtLoader.h>
#include <interrupts.h>
#include <stdint.h>

#pragma pack(push)  // pushea la alineación actual
#pragma pack(1)     // alinea todas las estructuras a un byte

// descriptor de una interrupcion
struct interrupt_descriptor_64
{
	uint16_t offset_l, selector;
	uint8_t cero, access;
	uint16_t offset_m;
	uint32_t offset_h, other_cero;
};

#pragma pack(pop)  // reestablece la alineación previa

typedef struct interrupt_descriptor_64* IDTEntry;

IDTEntry idt = (IDTEntry)0x0;  // IDT de 255 entradas

static void setup_idt_entry(int index, uint64_t offset);

void
idt_loader()
{
	asm_cli();

	// irqs
	setup_idt_entry(0x20, (uint64_t)&asm_irq00_handler);  // tick handler
	setup_idt_entry(0x21, (uint64_t)&asm_irq01_handler);  // keyboard handler

	// exceptions
	setup_idt_entry(0x00, (uint64_t)&asm_exception00_handler);
	setup_idt_entry(0x06, (uint64_t)&asm_exception06_handler);

	// syscall handler
	setup_idt_entry(0x80, (uint64_t)&asm_syscall_handler);

	// solo interrupción timer tick habilitadas
	asm_pic_master_mask(0xFC);
	asm_pic_slave_mask(0xFF);

	asm_sti();
}

static void
setup_idt_entry(int index, uint64_t offset)
{
	idt[index].selector = 0x08;
	idt[index].offset_l = offset & 0xFFFF;
	idt[index].offset_m = (offset >> 16) & 0xFFFF;
	idt[index].offset_h = (offset >> 32) & 0xFFFFFFFF;
	idt[index].access = ACS_INT;
	idt[index].cero = 0;
	idt[index].other_cero = (uint64_t)0;
}
