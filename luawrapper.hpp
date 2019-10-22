#pragma once

#ifndef LUAWRAPPER_HPP_
#define LUAWRAPPER_HPP_

#include "Arduino.h"

// Allow printing (eg with Serial) using the stream operator
template<class T> inline Print& operator <<(Print &obj, T arg) {
	obj.print(arg);
	return obj;
}
template<> inline Print& operator <<(Print &obj, float arg) {
	obj.print(arg, 4);
	return obj;
}

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"

char output[1024];

void runString(lua_State* L, const char lbuff[]) {
	int error = 0;
	// Serial.println("luaL_loadstring()");
	error = luaL_loadstring(L, lbuff);
	// Serial.println(error);
	if (error) {
		Serial << "Error Loading string: " << lbuff << "\n";
		sprintf(output, "%s", lua_tostring(L, -1));
		Serial.println(output);
		// Serial.println("lua_pop()");
		lua_pop(L, 1); /* pop error message from the stack */
	} else {
		// Serial.println("lua_pcall()");
		error = lua_pcall(L, 0, 0, 0);
		if (error) {
			Serial << "Error running:\n";
			sprintf(output, "%s", lua_tostring(L, -1));
			Serial.println(output);
			// Serial.println("lua_pop()");
			lua_pop(L, 1); /* pop error message from the stack */
		}
	}
}

	void runReplLoop(lua_State* L) {
		char repl_buffer[1024];
		char new_char;
		uint16_t buff_position = 0;



//  // Load global help() function
//  load_lua_string(help_function);
//
//  // Clear the stack
//  lua_settop(L, 0);
//  // Exec help() lua function
//  lua_getglobal(L, "help");
//  lua_call(L, 0, 0);  // 0, 0 = #args, #retvals

// prompt
		Serial << "\n>> ";
		while (1) {
			if (Serial.available() > 0) {
				new_char = Serial.read();
				if ((new_char >= 32 && new_char <= 127) // printable character
				|| new_char == '\n' // line break
				|| new_char == 4    // EOT end of transmission
				|| new_char == 8    // backspace
				|| new_char == 9    // tab
						) {
					repl_buffer[buff_position] = new_char;
					// echo new char
					Serial.write(new_char);
					buff_position++;
				}
				// TODO handle escape sequences 27 (and arrow keys)
			}

			// if no characters received skip the rest of the loop
			if (buff_position == 0)
				continue;

			// if backspace was pressed
			if (repl_buffer[buff_position - 1] == 8) {
				if (buff_position == 1)
					// just remove the backspace character
					buff_position--;
				else
					// remove both the backspace character and the previously entered character
					buff_position = buff_position - 2;
			}
			// if EOT end of transmission == 4 (Ctrl-D in miniterm.py)
			else if (repl_buffer[buff_position - 1] == 4) {
				// set the last character to the null char (should overwrite the EOT)
				repl_buffer[buff_position - 1] = '\0';
				// Serial << "\nGot: '" << repl_buffer << "'";
				Serial << '\n';

				runString(L, repl_buffer);

				// reset buffer index
				buff_position = 0;
				// erase repl_buffer?
				Serial << "\n>> ";
			}
		}

		// destroy lua vm
		// Serial.println("lua_close()");
		// lua_close(L);
	}


#endif /* LUAWRAPPER_HPP_ */
