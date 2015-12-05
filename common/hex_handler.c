#include "hex_handler.h"
#include "memory.h"
#include <string.h>

#define MAX_DATA_BYTES_PER_RECORD   (16)

enum record_type {
    RECORD_TYPE_DATA = 0x00,
    RECORD_TYPE_EOF = 0x01,
    //RECORD_TYPE_EXT_SEG = 0x02,
    //RECORD_TYPE_EXT_LINEAR = 0x04,
    //RECORD_TYPE_START_LIEAR = 0x05,
};

PACKED( 
struct record {
    uint8_t len;    //number of the data bytes
    union {
        addr_t addr;    //address to write this record
        struct { 
			//little endian
			uint8_t low_addr;
            uint8_t high_addr;  
        };
    };
    uint8_t type;   //record type
    uint8_t data[MAX_DATA_BYTES_PER_RECORD];    //binary data
    uint8_t checksum;   //checksum of the record
} );

struct record record_obuf = {
    .len = 0,
    .addr = 0,
    .type = RECORD_TYPE_DATA,
    .checksum = 0,
};

struct record record_ibuf = {
    .len = 0,
    .addr = 0,
    .type = RECORD_TYPE_DATA,
    .checksum = 0,
};

static void dump_record(FILE* code_file, const struct record* record);
static uint8_t checksum(const struct record* record);
static bool read_record(struct record* record,  FILE* code_file);
static bool load_record(mem_t* mem, struct record* record);

static inline uint8_t record_size(const struct record* record)
{
	return sizeof(uint8_t) * record->len
		+ sizeof(record->len)
		+ sizeof(record->addr)
		+ sizeof(record->type)
		+ sizeof(record->checksum);
}

static inline bool is_valid(char ch)
{
	if ('0' <= ch && ch <= '9')
		return true;
	else if ('a' <= ch && ch <= 'z')
		return true;
	else if ('A' <= ch && ch <= 'Z')
		return true;
	return false;
}

static inline uint8_t ch2u8(char ch)
{
	if ('0' <= ch && ch <= '9')
		return ch - '0';
	else if ('a' <= ch && ch <= 'z')
		return ch - 'a' + 10;
	else if ('A' <= ch && ch <= 'Z')
		return ch - 'A' + 10;
	return 0;
}

/**dump one instruction to hex file
@param: code, instruction in little endian
@param: len, length of the instruction
@param: addr, the address to store the instruction or data
*/
void dump_code(FILE* code_file, uint8_t code[3], uint8_t code_len, addr_t addr)
{
    //static addr_t ext_addr = 0x0000;
    uint8_t offset = record_obuf.len;
    //not continuous address, need a new record
    if (offset + record_obuf.addr != addr || \
        offset + code_len > MAX_DATA_BYTES_PER_RECORD) { 
        ASSERT(RECORD_TYPE_DATA == record_obuf.type || \
            RECORD_TYPE_EOF == record_obuf.type);
        dump_record(code_file, &record_obuf);
		
		//init new record
		record_obuf.len = 0;
		record_obuf.addr = addr;
    }
	
	memcpy((void*)(record_obuf.data + record_obuf.len), (void*)code, code_len);
	record_obuf.len += code_len;
}

//this must be call the end the dump
void dump_end(FILE* code_file)
{
    if (0 != record_obuf.len) { //code lefted in buffer, need dump
        dump_record(code_file, &record_obuf);
    }

    //dump eof record
	record_obuf.addr = 0;
    record_obuf.len = 0;
    record_obuf.type = RECORD_TYPE_EOF;
    dump_record(code_file, &record_obuf);
}

static void dump_record(FILE* code_file, const struct record* record)
{
    uint8_t i;
	if (0 == record->len && RECORD_TYPE_EOF != record->type) //no data to dump
		return;
    fprintf(code_file, ":");
    
    fprintf(code_file, "%02x", record->len);
    
    fprintf(code_file, "%04x", record->addr);
    fprintf(code_file, "%02x", record->type);
    for (i = 0; i < record->len; i++) {
        fprintf(code_file, "%02x", record->data[i]);
    }
    fprintf(code_file, "%02x", checksum(record));
    fprintf(code_file, "\n");
}

static uint8_t checksum(const struct record* record)
{
    uint8_t i;
    uint8_t checksum = 0;
    checksum += record->len;
    checksum += record->high_addr;
    checksum += record->low_addr;
    checksum += record->type;
    for (i = 0; i < record->len; i++) {
            checksum += record->data[i];
        }
    checksum = 0x01 + ~checksum;
    return checksum;
}

//read one record from the hex file
static bool read_record(struct record* record,  FILE* code_file)
{
    size_t cnt;
	const size_t max_cnt = 2 * sizeof(struct record);
    uint8_t* p;
    char ch = fgetc(code_file);
    if (EOF == ch || ':' != ch)
        return false;
    cnt = 0;
    p = (uint8_t*)record;
    for (;;) {
        ch = fgetc(code_file);
        if (EOF == ch || '\n' == ch) {
            break;
        } else if ('\r' == ch) {
            ch = fgetc(code_file);
            break;
        }
        if (cnt > max_cnt || !is_valid(ch))
            return false;
		ch  = ch2u8(ch);
        
        if (0 == (cnt & 1))
            p[cnt >> 1] = (0xF0 & (ch << 4)) | (0x0F & p[cnt >> 1]);
        else
            p[cnt >> 1] = (p[cnt >> 1] & 0xF0) | (0x0F & ch);
        ++cnt;
    }
    //the last two chars should be checksum
	record->checksum = p[(cnt >> 1) - 1];
	//need to convert to cpu represent
	record->addr = le16_to_cpu(record->addr);
    if (record_size(record) * 2 != cnt)
        return false;
    if (record->checksum != checksum(record))
        return false;
    return true;
}

static bool load_record(mem_t* mem, struct record* record)
{
    uint8_t i;
    addr_t addr = record->addr;
    if (RECORD_TYPE_EOF == record->type)
        return true;
    for (i = 0; i < record->len; i++) {
        write_byte(addr + i, record->data[i]);
    }
    return true;
}

bool load(mem_t* mem, FILE* code_file)
{
    while (read_record(&record_ibuf, code_file)) {
        load_record(mem, &record_ibuf);
        if (RECORD_TYPE_EOF == record_ibuf.type)
            break;
		record_ibuf.len = 0;
		record_ibuf.addr = 0;
		record_ibuf.type = RECORD_TYPE_DATA;
		record_ibuf.checksum = 0;
    }
    if (RECORD_TYPE_EOF != record_ibuf.type) {
        error("invalid input hex file!\n");
        return false;
    }

    return true;
}
