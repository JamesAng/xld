/*
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR /PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/bits.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/sys_info.h>

/*
 *  get_device_type(): tell if GP/HS/EMU/TST
 */
u32 get_device_type(void)
{
	int mode;
	mode = __raw_readl(CONTROL_STATUS) & (DEVICE_MASK);
	return mode >>= 8;
}

/*
 *  get_cpu_type(): extract cpu info
 */
u32 get_cpu_type(void)
{
	return __raw_readl(CONTROL_OMAP_STATUS);
}

/*
 * get_cpu_id(): extract cpu id
 * returns 0 for ES1.0, cpuid otherwise
 */
u32 get_cpu_id(void)
{
	u32 cpuid = 0;

	/*
	 * On ES1.0 the IDCODE register is not exposed on L4
	 * so using CPU ID to differentiate between ES1.0 and > ES1.0.
	 */
	__asm__ __volatile__("mrc p15, 0, %0, c0, c0, 0":"=r"(cpuid));
	if ((cpuid & 0xf) == 0x0) {
		return 0;
	} else {
		/* Decode the IDs on > ES1.0 */
		cpuid = __raw_readl(CONTROL_IDCODE);
	}

	return cpuid;
}

/*
 * get_cpu_family(void): extract cpu info
 */
u32 get_cpu_family(void)
{
	u16 hawkeye;
	u32 cpu_family;
	u32 cpuid = get_cpu_id();

	if (cpuid == 0)
		return CPU_OMAP34XX;

	hawkeye = (cpuid >> HAWKEYE_SHIFT) & 0xffff;
	switch (hawkeye) {
	case HAWKEYE_OMAP34XX:
		cpu_family = CPU_OMAP34XX;
		break;
	case HAWKEYE_AM35XX:
		cpu_family = CPU_AM35XX;
		break;
	case HAWKEYE_OMAP36XX:
		cpu_family = CPU_OMAP36XX;
		break;
	default:
		cpu_family = CPU_OMAP34XX;
	}

	return cpu_family;
}

/*
 * get_cpu_rev(void): extract version info
 */
u32 get_cpu_rev(void)
{
	u32 cpuid = get_cpu_id();

	if (cpuid == 0)
		return CPU_3XX_ES10;
	else
		return (cpuid >> CPU_3XX_ID_SHIFT) & 0xf;
}
