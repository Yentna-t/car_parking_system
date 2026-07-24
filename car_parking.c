#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 0 = ว่าง (Free), 1 = มีรถจอด (Occupied), 2 = ล็อกชั่วคราว (Held)
#define STATUS_FREE 0
#define STATUS_OCCUPIED 1
#define STATUS_HELD 2

struct ParkingSlot {
    int status;
    char ticket_id[15];
    char license_plate[20]; // ทะเบียนรถ
    time_t checkin_time;
};

// ฟังก์ชันคำนวณค่าบริการ
int calculate_fee(time_t checkin, time_t checkout, int *overtime_hours) {
    int base_fee = 20000;
    *overtime_hours = 0;

    struct tm *out_tm = localtime(&checkout);
    
    // ตรวจสอบว่าเวลาออก เกิน 20:00 น. หรือไม่
    if (out_tm->tm_hour >= 20000) {
        int extra = (out_tm->tm_hour - 20000) + (out_tm->tm_min > 0 ? 1 : 0);
        *overtime_hours = extra;
    }

    return base_fee + (*overtime_hours * 10000);
}

// ฟังก์ชันวาด QR Code จริง (LAO QR / EMVCo) แบบ ASCII - สแกนได้บนพื้นหลังสีดำ
void display_qr_code(int amount) {
    printf("\n=================================================================================\n");
    printf("   SCAN LAO QR TO PAY %d LAK\n", amount);
    printf("=================================================================================\n");
    printf("██████████████████████████████████████████████████████████████████████████\n");
    printf("██████████████████████████████████████████████████████████████████████████\n");
    printf("████              ██    ██  ██          ██  ██        ██              ████\n");
    printf("████  ██████████  ██      ██        ████████████████  ██  ██████████  ████\n");
    printf("████  ██      ██  ██    ██    ████      ██  ██    ██████  ██      ██  ████\n");
    printf("████  ██      ██  ████  ██████    ██          ██      ██  ██      ██  ████\n");
    printf("████  ██      ██  ██  ██    ████      ██        ██  ████  ██      ██  ████\n");
    printf("████  ██████████  ████  ████████  ████  ██          ████  ██████████  ████\n");
    printf("████              ██  ██  ██  ██  ██  ██  ██  ██  ██  ██              ████\n");
    printf("████████████████████████      ████████  ██  ██  ██  ██████████████████████\n");
    printf("████  ████              ██    ████    ██  ██  ██        ████  ██      ████\n");
    printf("████  ██  ██    ████  ██    ██████  ████  ██████  ████  ██  ████  ████████\n");
    printf("██████    ██████      ████  ████    ██    ██████        ██    ████    ████\n");
    printf("████      ██  ████  ██  ████  ████  ██    ████████    ████████        ████\n");
    printf("████          ██  ██  ██          ██████      ██  ████    ██    ██  ██████\n");
    printf("██████  ██████  ██      ██        ████  ████  ██    ██        ██      ████\n");
    printf("████████████  ██  ████          ██  ██████          ██  ██  ████    ██████\n");
    printf("████            ██      ██  ██  ██  ██    ██████████████    ██    ██  ████\n");
    printf("████  ██  ████    ██████      ██  ████  ████      ██  ████        ██  ████\n");
    printf("████    ██████  ██          ██  ██████  ██████        ██████  ██  ████████\n");
    printf("████████  ██      ████████  ████████  ██████  ██████                  ████\n");
    printf("████  ██        ██  ████  ████    ████  ██    ██████  ██  ██  ██    ██████\n");
    printf("██████  ██  ██        ██  ██  ██      ██      ██  ██    ██    ██  ████████\n");
    printf("████  ██████  ████          ████      ████      ██  ██  ██        ██  ████\n");
    printf("████  ██    ████    ██      ██      ██          ████  ██    ██  ████  ████\n");
    printf("████  ██  ████████      ████████    ██  ██████      ████      ██    ██████\n");
    printf("████    ██    ██  ██  ██  ████    ██      ██  ██████            ████  ████\n");
    printf("████████████████████    ██    ████        ██    ████  ██████  ████████████\n");
    printf("████              ██  ██      ██    ██████            ██  ██  ████████████\n");
    printf("████  ██████████  ██  ██      ██████      ██████████  ██████  ██  ████████\n");
    printf("████  ██      ██  ██    ██  ██  ██████      ██  ████          ██████  ████\n");
    printf("████  ██      ██  ██    ████████████      ██████      ████      ██████████\n");
    printf("████  ██      ██  ██████    ██  ██  ████        ██        ████        ████\n");
    printf("████  ██████████  ████    ██  ██  ████        ██  ████  ██████  ████  ████\n");
    printf("████              ██  ██  ██    ██    ██    ████          ██    ██  ██████\n");
    printf("██████████████████████████████████████████████████████████████████████████\n");
    printf("██████████████████████████████████████████████████████████████████████████\n");
    printf("=================================================================================\n");
    printf("   * QR : ສະແກນແລ້ວປ່ອນຍອດ %d ກີບ (LAK) ດ້ວຍຕົນເອງ *\n", amount);
    printf("=================================================================================\n");
}

int main() {
    struct ParkingSlot parking[5];
    
    // กำหนดค่าเริ่มต้นให้ทุกช่องว่าง
    for(int i = 0; i < 5; i++) {
        parking[i].status = STATUS_FREE;
        strcpy(parking[i].ticket_id, "");
        strcpy(parking[i].license_plate, "");
    }

    int choice;

    while(1) {
        time_t now = time(NULL);
        struct tm *current_time = localtime(&now);

        printf("\n============================================\n");
        printf("    PARKING SYSTEM (Current Time: %02d:%02d)\n", current_time->tm_hour, current_time->tm_min);
        printf("============================================\n");

        int available_count = 0;
        for(int i = 0; i < 5; i++) {
            if(parking[i].status == STATUS_FREE) {
                printf("ຊ່ອງ %d: [ Empty ]\n", i + 1);
                available_count++;
            } else if(parking[i].status == STATUS_HELD) {
                printf("ຊ່ອງ %d: [ HOLD - ລ໋ອຊົ່ວຄາວ ]\n", i + 1);
            } else {
                printf("ຊ່ອງ %d: [ Full ] (Ticket: %s | ເລກທະບຽນ: %s)\n", i + 1, parking[i].ticket_id, parking[i].license_plate);
            }
        }

        if(available_count == 0) {
            printf("\n>>> [ FULL ] ຂະນະນີ້ຊ່ອງຈອດລົດເຕັມ <<<\n");
        }

        printf("\n1. Check-in (ນຳລົດເຂົ້າຈອດ)\n");
        printf("2. Check-out (ນຳລົດອອກ / ປ້ອນປີ້ເຂົ້າເຄື່ອງ)\n");
        printf("0. ອອກຈາກໂປຣເເກຣມ\n");
        printf("ເລືອກລາຍການ: ");
        scanf("%d", &choice);

        if(choice == 0) {
            printf("ປິດລະບົບຮຽບຮ້ອຍ\n");
            break;
        }

        // --- 1. CHECK-IN (พร้อมระบบ HOLD SPOT) ---
        if(choice == 1) {
            if(available_count == 0) {
                printf("ບໍ່ສາມາດຈອດໄດ້ ບ່ອນຈອດລົດເຕັມແລ້ວ!!!\n");
                continue;
            }

            int slot;
            printf("ເລືອກຊ່ອງຈອດ (1-5): ");
            scanf("%d", &slot);

            if(slot < 1 || slot > 5) {
                printf("ເລືອກຊ່ອງຈອດບໍ່ຖືກຕ້ອງ!\n");
                continue;
            }

            int idx = slot - 1;
            if(parking[idx].status != STATUS_FREE) {
                printf("ຊ່ອງນີ້ບໍ່ວ່າງ ຫຼື ບໍ່ສາມາດຈອດໄດ້!\n");
                continue;
            }

            // 🟡 Step 1: Hold Spot (ล็อกช่องชั่วคราว)
            parking[idx].status = STATUS_HELD;
            printf("\n[HOLD] ລ໋ອກຊ່ອງຈອດທີ່ %d ຊົ່ວຄາວຮຽບຮ້ອຍ...\n", slot);

            char plate[20];
            printf("ປ້ອນທະບຽນລົດ (ເລກ 4 ຫຼັກ ເຊັ່ນ 1234): ");
            scanf("%s", plate);

            // ตรวจสอบว่าเป็นตัวเลข 4 หลักหรือไม่
            int valid_plate = 1;
            if(strlen(plate) != 4) {
                valid_plate = 0;
            } else {
                for(int i = 0; i < 4; i++) {
                    if(plate[i] < '0' || plate[i] > '9') {
                        valid_plate = 0;
                        break;
                    }
                }
            }

            if(!valid_plate) {
                parking[idx].status = STATUS_FREE; // คืนช่องที่ HOLD ไว้
                printf("ທະບຽນລົດບໍ່ຖືກຕ້ອງ! ຕ້ອງເປັນເລກ 4 ຫຼັກເທົ່ານັ້ນ\n");
                printf("ຄືນຊ່ອງຈອດ %d ເປັນ [Empty] ຮຽບຮ້ອຍ\n", slot);
                continue;
            }

            printf("ຕ້ອງການຍືນຍັນເພື່ອຮັບປີ້ ຫຼື ບໍ່?\n");
            printf("1. ຍືນຍັນ (ອອກປີ້)\n");
            printf("0. ຍົກເລິກ (ຄືນຊ່ອງຈອດ)\n");
            printf("ເລືອກ: ");
            int confirm;
            scanf("%d", &confirm);

            if(confirm == 1) {
                // 🟢 Step 2: ยืนยันสำเร็จ เปลี่ยนสถานะเป็น Occupied
                parking[idx].status = STATUS_OCCUPIED;
                parking[idx].checkin_time = time(NULL);
                sprintf(parking[idx].ticket_id, "TK-%02d-%d", slot, rand() % 900 + 100);
                strcpy(parking[idx].license_plate, plate);

                printf("\n---------------------------------\n");
                printf(" Check-in สำเร็จ!\n");
                printf(" ຊ່ອງຈອດ: %d\n", slot);
                printf(" ທະບຽນລົດ: %s\n", parking[idx].license_plate);
                printf(" ລະຫັດປີ້ (Ticket ID): %s\n", parking[idx].ticket_id);
                printf("---------------------------------\n");
            } else {
                // 🔴 Step 3: ยกเลิกการทำรายการ คืนสถานะเป็น Free
                parking[idx].status = STATUS_FREE;
                printf("ຍົກເລິກເຮັດລາຍການ ຄືນຊ່ອງຈອດ %d ເປັນ [Empty] ຮຽບຮ້ອຍ\n", slot);
            }
        }

        // --- 2. CHECK-OUT & PAYMENT ---
        else if(choice == 2) {
            char input_code[20];
            printf("ປ້ອນ Ticket ID ຫຼື ທະບຽນລົດ: ");
            scanf("%s", input_code);

            int found_idx = -1;
            for(int i = 0; i < 5; i++) {
                if(parking[i].status == STATUS_OCCUPIED &&
                   (strcmp(parking[i].ticket_id, input_code) == 0 ||
                    strcmp(parking[i].license_plate, input_code) == 0)) {
                    found_idx = i;
                    break;
                }
            }

            if(found_idx == -1) {
                printf("ບໍ່ພົບລົດທີ່ຢູ່ໃນລະບົບ!\n");
                continue;
            }

            time_t checkout_time = time(NULL);
            int overtime_hours = 0;
            int total_fee = calculate_fee(parking[found_idx].checkin_time, checkout_time, &overtime_hours);

            printf("\n=================================\n");
            printf("         BILL RECEIPT\n");
            printf("=================================\n");
            printf("ຊ່ອງຈອດ: %d\n", found_idx + 1);
            printf("ທະບຽນລົດ: %s\n", parking[found_idx].license_plate);
            printf("ຄ່າຈອດພື້ນຖານ: 20 000 ກີບ\n");
            if(overtime_hours > 0) {
                printf("ຄ່າບໍລິການເກີນເວລາ 20:00 ນ. (%d ຊມ. x 10 000 ກີບ): %d ກີບ\n", overtime_hours, overtime_hours * 10000);
            }
            printf("ຍອດຊຳລະ: %d ກີບ\n", total_fee);

            printf("\nຊ່ອງທາງການຊຳລະເງິນ:\n");
            printf("1. ເງິນສົດ (Cash)\n");
            printf("2. ສະແກນໂອນ (PromptPay QR)\n");
            printf("ເລືອກ: ");
            int pay_type;
            scanf("%d", &pay_type);

            if(pay_type == 1) {
                int cash;
                printf("ຮັບເງຶນມາ (ກີບ): ");
                scanf("%d", &cash);
                if(cash >= total_fee) {
                    int change = cash - total_fee;
                    printf("ຊຳລະສຳເລັດ! ເງິນທອນ: %d ກີບ\n", change);

                    // คืนช่องจอด
                    parking[found_idx].status = STATUS_FREE;
                    strcpy(parking[found_idx].ticket_id, "");
                    strcpy(parking[found_idx].license_plate, "");
                } else {
                    printf("ຈຳນວນເງິນບໍ່ພຽງພໍ! ການຊຳລະເງິນລົ້ມເລວ ລົດຍັງຄົງຢູ່ຊ່ອງຈອດ\n");
                }
            } else if(pay_type == 2) {
                display_qr_code(total_fee);
                printf("ກົດ 1 ເພື່ອຍືນຍັນການຊຳລະ (0 ເພື່ອຍົກເລິກ): ");
                int confirm;
                scanf("%d", &confirm);
                if(confirm == 1) {
                    printf("ຊຳລະຜ່ານ QR Code ສຳເລັດ!\n");

                    // คืนช่องจอด
                    parking[found_idx].status = STATUS_FREE;
                    strcpy(parking[found_idx].ticket_id, "");
                    strcpy(parking[found_idx].license_plate, "");
                } else {
                    printf("ການໂອນລົ້ມເລວ ຫຼື ຖືກຍົກເລີກ!\n");
                }
            }
        }
    }

    return 0;
}