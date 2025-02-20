
int buffer_read_index = 0;
int buffer_write_index = 0;
char buffer[1000] = "";

#define RELEASE_CHAR(a) ((a) & 0x80)
#define CAPS 0x3A
#define LCTRL 0x1D
#define LALT 0x38
#define LSHIFT 0x2A
#define RCTRL
#define RALT
#define RSHIFT


// 点代表非可见字符, 或者是"."本身
const char scancode_table[] =       "..1234567890-=\b.qwertyuiop[]\n.asdfghjkl;\'`.\\zxcvbnm,./.*. .............7894561230......";
const char shift_scancode_table[] = "..!@#$%^&*()_+\b.QWERTYUIOP{}\n.ASDFGHJKL:\"~.|ZXCVBNM<>?.*. .............7894561230......";
// const char caps_scancode_table[] =  "..1234567890-=\b.QWERTYUIOP[]\n.ASDFGHJKL;\'`.\\ZXCVBNM,./.*. .............7894561230......";

// This function is not supposed
// to be here but just for convenience
char toupper(char ch) {
	if (ch >= 'a' && ch <= 'z') {
		ch = (char)(ch - 'a' + 'A');
	}
	return ch;
}

void do_keyboard_interrupt(short scancode) {
	// Define some flags
	static char caps = 0;
	static char lshift = 0;
	static char rshift = 0;
	static char lctrl = 0;
	static char rctrl = 0;
	static char lalt = 0;
	static char ralt = 0;
	static char cap_out = 0;
	char ch = ' ';

	if (RELEASE_CHAR(scancode)) {

		// Check if Shift / Alt / Ctrl / Released
		if (scancode == (LCTRL | 0x80)) {
			lctrl = 0;
		}
		if (scancode == (LALT | 0x80)) {
			lalt = 0;
		}
		if (scancode == (LSHIFT | 0x80)) {
			lshift = 0;
		}
		return ;
	}
	if (!RELEASE_CHAR(scancode)) {
		if (scancode == LCTRL) {
			lctrl = 1;
			return ;
		}
		if (scancode == LALT) {
			lalt = 1;
			return ;
		}
		if (scancode == LSHIFT) {
			lshift = 1;
			return ;
		}
		if (scancode == CAPS) {
			caps = !caps;
			return ;
		}
		cap_out = caps?!(lshift || rshift):(lshift || rshift);
		if (lshift || rshift) {
			ch = shift_scancode_table[scancode];
			if (cap_out) {
				ch = toupper(ch);
			}
		}
		else if (lctrl || rctrl) {
			// here we need ctrl escape
		}
		else if (lalt || ralt) {
			// here we do nothing :/
		}
		else {
			ch = scancode_table[scancode];
			if(cap_out) {
				ch = toupper(ch);
			}
		}
		// TODO: 使得 tty 和进程对应, 当前都是指向 tty_table[0]
		// handle ch
		video_putchar(ch);
		buffer[buffer_write_index] = ch;
		buffer_write_index++;
		if(ch == '\n'){
			buffer_read_index = buffer_write_index;
		} else {

		}
		if(buffer_write_index >= 1000){
			buffer_write_index = 0;
		}
	}
	// Wakeup the buffer queue
	// 我们只有在收到 \n EOF 的时候才唤醒队列
	// wake_up(&tty_table[0].buffer.wait_proc);
	return ;
}

