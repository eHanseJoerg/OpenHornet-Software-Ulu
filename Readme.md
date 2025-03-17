/**************************************************************************************
 *        ____                   _    _                       _
 *       / __ \                 | |  | |                     | |
 *      | |  | |_ __   ___ _ __ | |__| | ___  _ __ _ __   ___| |_
 *      | |  | | '_ \ / _ \ '_ \|  __  |/ _ \| '__| '_ \ / _ \ __|
 *      | |__| | |_) |  __/ | | | |  | | (_) | |  | | | |  __/ |_
 *       \____/| .__/ \___|_| |_|_|  |_|\___/|_|  |_| |_|\___|\__|
 *             | |
 *             |_|
 *
 *   Project OpenHornet
 *
 *   The OpenHornet Project is a F/A-18C OFP 13C Lot 20 1:1 Replica Simulator,
 *   consisting of a physical structure and electrical/software interfaces to a PC
 *   to be driven by Digital Combat Simulator (DCS).
 *
 *   ---------------------------------------------------------------------------------
 *
 *   This Project is released under the Creative Commons
 *   Atribution - Non Comercal - Share Alike License.: CC BY-NC-SA 3.0
 *   More Information: https://creativecommons.org/licenses/by-nc-sa/3.0/
 *
 *   ---------------------------------------------------------------------------------
 *
 *   The OpenHornet Software is based on DCS-BIOS
 *   More information: http://dcs-bios.a10c.de
 *
 *   DCS-BIOS is released under the following terms:
 *   https://github.com/dcs-bios/dcs-bios/blob/develop/DCS-BIOS-License.txt
 *
 *   Use only the following DCS-BIOS Arduino Library
 *   https://github.com/DCSFlightpanels/dcs-bios-arduino-library
 *
 **************************************************************************************/

/**
 * @file OH_LIP_BL_controller.ino
 * @author Higgins, Ulukaii, Arribe
 * @date <10.10.2024>
 * @version 0.9
 * @warning Before using this code, check your panel versions and LED counts.
 * This code uses: HUD PANEL v.2 (56 instead of 50   *  LEDs), SPIN RCVY PANEL v2.1
 * (63 LEDs, Spin indicator two non-subsequent LEDs) and ANT SEL v2 (13 LEDs).
 * @brief Backlight controller code. Intended board:
 * ABSIS BACKLIGHT CONTROLLER. Processor: Arduino Mega + Shield
 * Not implemented yet:
 * - Logic function to check whether DCS is running
 * - WARN/CAUT brightness
 * - Rotary encoder on SIM PWR PANEL
 */