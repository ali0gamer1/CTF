

import string
stack = [0] * 128
sp = 0
pc = 0
file = open("raw_bytes_unedited","r")
program = []
for a in file:
    program.append(a.strip())
execute = True
flag = "I dont know lol"
count =  0
jmp_flag = False
jmp_addr = 0
inp_flag = False
the_list = []
while execute:

    count += 1
    if pc >= 1490:
        print("End of program")
        break

    if sp < 0:
        print("Stack underflow")
        break

    

    if jmp_flag and (pc == jmp_addr):
        jmp_flag = False
        print("ok")
    #This is a test and doesn't need improvment
    if not jmp_flag:
        cmd = input(f"opcode: {program[pc]}\tsp: {sp}\tpc: {pc}\nTotal executed: {count - 1}\n")
    if "stack" in cmd and not jmp_flag:
        ls = []
        if "hex" in cmd:
            for i in stack:
                ls.append(hex(i))
        else:
            ls = stack
        print("==================")
        print(ls)
        print("==================")
        count -= 1
        continue
    if "jmp " in cmd and not jmp_flag:
        jmp_flag = True
        jmp_addr = int(cmd[4:])
        print(jmp_addr)
        continue
    

    
    if program[pc] == "01":
        execute = 0
        print("Exit")
        print(the_list)
        continue

    if program[pc] == "81":
        value = int(program[pc + 2] + program[pc + 1], 16)
        stack[sp] = value
        if inp_flag:
            the_list.append(f"{pc}:\tpush {value}")
        sp += 1
        pc += 3
        continue

    elif program[pc] == "80":
        stack[sp] = int(program[pc + 1],16)
        if inp_flag:
            the_list.append(f"{pc}:\tpush {stack[sp]}")
        sp += 1
        pc += 2
        continue
    
    elif program[pc] == "30": #Unconditional jump
        if inp_flag:
            the_list.append(f"{pc}:\tjmp {stack[sp - 1]}")
        pc = stack[sp - 1]
        sp -= 1
        continue
    
    elif program[pc] == "10": #Copy
        temp_sp = sp
        temp_sp2 = sp - 1
        sp += 1
        stack[temp_sp] = stack[temp_sp2]
        pc += 1
        if inp_flag:
            the_list.append(f"{pc - 1}:\tcopy_to_top_of_the_stack")
        continue

    elif program[pc] == "11":
        sp -= 1
        pc += 1
        if inp_flag:
            the_list.append(f"{pc - 1}:\tdec sp")
        continue

    elif program[pc] == "12": #Add 
        temp_sp = sp
        temp_var = stack[temp_sp - 1]
        temp_var2 = stack[temp_sp - 2]
        stack[temp_sp - 2] = temp_var + temp_var2
        sp -= 1
        pc += 1
        if inp_flag:
            the_list.append(f"{pc - 1}:\tadd [sp - 2], [sp - 1]")
        continue

    elif program[pc] == "13": #Subtract 
        temp_sp = sp
        temp_var = stack[temp_sp - 1]
        temp_var2 = stack[temp_sp - 2]
        stack[temp_sp - 2] = temp_var2 - temp_var
        sp -= 1
        pc += 1
        if inp_flag:
            the_list.append(f"{pc - 1}:\tsub [sp - 2], [sp - 1]")
        continue

    elif program[pc] == "14": #swap
        temp = stack[temp_sp - 2]
        stack[temp_sp - 2] = stack[temp_sp - 1]
        stack[temp_sp - 1] = temp
        if inp_flag:
            the_list.append(f"{pc}:\tswap [sp - 2], [sp - 1]")
        pc += 1
        continue

    elif program[pc] == "c0":
        inp_flag = True
        print(pc)
        inp = input('Enter a value (len = 1)')
        if len(inp) != 1:
            continue
        stack[sp] = ord(inp)
        sp += 1
        pc += 1
        if inp_flag:
            the_list.append(f"{pc - 1}:\tinput")
    
    elif program[pc] == "31": #Conditional jmp
        cond = stack[sp - 2]
        addr = stack[sp - 1]

        if inp_flag:
            the_list.append(f"{pc}:\tjmp_ if_[sp - 2] == 0 {addr}")
            
        if cond == 0:
            pc = addr
        else:
            pc += 1

        
        sp -= 2
    
    elif program[pc] == "c1":
        val = chr(stack[sp - 1])
        if val not in string.ascii_letters + string.punctuation:
            val = f"Value = '{hex(stack[sp - 1])}'"
        else:
            val = f"Value = '{chr(stack[sp - 1])}'"

        print(f"Function call with param: {val}")
        sp -= 1
        pc += 1
        if inp_flag:
            the_list.append(f"{pc - 1}:\tcall({val})")

    else:
        print(f"COUNT: {count}")
        print(f"PC: {pc}")
        print(f"Unexpected opcode {program[pc]}")
        input()  
    

    
    
