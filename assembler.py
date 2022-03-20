#!/usr/bin/python3
import base64
import re
import sys
from colorama import Style, Fore


def error(msg):
    print(f"{Fore.RED}{msg}{Style.RESET_ALL}")

def to_bin(i, n):
    return "{:0{}b}".format(i, n)

def isempty(text):
    for i in range(len(text)):
        if i != " " and i != "\n" and i != "\t":
            return False
    return True

def getr1r2x3(operands, line):
    if len(operands) != 3:
        error(f"Line {line+1}: Invalid number of operands")
        return None

    op1, op2, op3 = operands
    op1 = op1.strip()
    op2 = op2.strip()
    op3 = op3.strip()

    if not op1.startswith("R") or not op2.startswith("R"):
        error(f"Line {line+1}: Invalid operands")
        return None
    
    op1 = op1[1:]
    op2 = op2[1:]

    if not op1.isdigit() or not int(op1) < 64 or not int(op1) >= 0:
        error(f"Line {line+1}: Invalid operands")
        return None

    if not op2.isdigit() or not int(op2) < 64 or not int(op2) >= 0:
        error(f"Line {line+1}: Invalid operands")
        return None

    op1, op2 = int(op1), int(op2)
    return op1, op2, op3

def chalt(operands, line, condition):
    return bytes([0, 0, 0])

def cadd(operands, line, condition):
    try:
        op1, op2, op3 = getr1r2x3(operands, line)
    except ValueError:
        return None

    if op3.startswith("R") and op3[1:].isdigit() and int(op3[1:]) < 64 and int(op3[1:]) >= 0:
        op3 = int(op3[1:])
        b = to_bin(1 + 21 * condition, 6) + to_bin(op1, 6) + to_bin(op2, 6) + to_bin(op3, 6)
        
    elif op3.startswith("#") and op3[1:].isdigit() and int(op3[1:]) < 64 and int(op3[1:]) >= 0:
        op3 = int(op3[1:])
        b = to_bin(2 + 21 * condition, 6) + to_bin(op1, 6) + to_bin(op2, 6) + to_bin(op3, 6)

    else:
        error(f"Line {line+1}: Invalid operands")
        return None

    b1 = int(b[:8], 2)
    b2 = int(b[8:16], 2)
    b3 = int(b[16:], 2)
    return bytes([b1, b2, b3])

def csub(operands, line, condition):
    try:
        op1, op2, op3 = getr1r2x3(operands, line)
    except ValueError:
        return None

    if op3.startswith("R") and op3[1:].isdigit() and int(op3[1:]) < 64 and int(op3[1:]) >= 0:
        op3 = int(op3[1:])
        b = to_bin(3 + 21 * condition, 6) + to_bin(op1, 6) + to_bin(op2, 6) + to_bin(op3, 6)

    else:
        error(f"Line {line+1}: Invalid operands")
        return None

    b1 = int(b[:8], 2)
    b2 = int(b[8:16], 2)
    b3 = int(b[16:], 2)
    return bytes([b1, b2, b3])

def cor(operands, line, condition):
    try:
        op1, op2, op3 = getr1r2x3(operands, line)
    except ValueError:
        return None

    if op3.startswith("R") and op3[1:].isdigit() and int(op3[1:]) < 64 and int(op3[1:]) >= 0:
        op3 = int(op3[1:])
        b = to_bin(5 + 21 * condition, 6) + to_bin(op1, 6) + to_bin(op2, 6) + to_bin(op3, 6)
        
    elif op3.startswith("#") and op3[1:].isdigit() and int(op3[1:]) < 64 and int(op3[1:]) >= 0:
        op3 = int(op3[1:])
        b = to_bin(6 + 21 * condition, 6) + to_bin(op1, 6) + to_bin(op2, 6) + to_bin(op3, 6)

    else:
        error(f"Line {line+1}: Invalid operands")
        return None

    b1 = int(b[:8], 2)
    b2 = int(b[8:16], 2)
    b3 = int(b[16:], 2)
    return bytes([b1, b2, b3])

def cxor(operands, line, condition):
    try:
        op1, op2, op3 = getr1r2x3(operands, line)
    except ValueError:
        return None

    if op3.startswith("R") and op3[1:].isdigit() and int(op3[1:]) < 64 and int(op3[1:]) >= 0:
        op3 = int(op3[1:])
        b = to_bin(7 + 21 * condition, 6) + to_bin(op1, 6) + to_bin(op2, 6) + to_bin(op3, 6)
        
    elif op3.startswith("#") and op3[1:].isdigit() and int(op3[1:]) < 64 and int(op3[1:]) >= 0:
        op3 = int(op3[1:])
        b = to_bin(8 + 21 * condition, 6) + to_bin(op1, 6) + to_bin(op2, 6) + to_bin(op3, 6)

    else:
        error(f"Line {line+1}: Invalid operands")
        return None

    b1 = int(b[:8], 2)
    b2 = int(b[8:16], 2)
    b3 = int(b[16:], 2)
    return bytes([b1, b2, b3])

def cand(operands, line, condition):
    try:
        op1, op2, op3 = getr1r2x3(operands, line)
    except ValueError:
        return None

    if op3.startswith("R") and op3[1:].isdigit() and int(op3[1:]) < 64 and int(op3[1:]) >= 0:
        op3 = int(op3[1:])
        b = to_bin(9 + 21 * condition, 6) + to_bin(op1, 6) + to_bin(op2, 6) + to_bin(op3, 6)
        
    elif op3.startswith("#") and op3[1:].isdigit() and int(op3[1:]) < 64 and int(op3[1:]) >= 0:
        op3 = int(op3[1:])
        b = to_bin(10 + 21 * condition, 6) + to_bin(op1, 6) + to_bin(op2, 6) + to_bin(op3, 6)

    else:
        error(f"Line {line+1}: Invalid operands")
        return None

    b1 = int(b[:8], 2)
    b2 = int(b[8:16], 2)
    b3 = int(b[16:], 2)
    return bytes([b1, b2, b3])

def cshl(operands, line, condition):
    try:
        op1, op2, op3 = getr1r2x3(operands, line)
    except ValueError:
        return None

    if op3.startswith("R") and op3[1:].isdigit() and int(op3[1:]) < 64 and int(op3[1:]) >= 0:
        op3 = int(op3[1:])
        b = to_bin(12 + 21 * condition, 6) + to_bin(op1, 6) + to_bin(op2, 6) + to_bin(op3, 6)
        
    elif op3.startswith("#") and op3[1:].isdigit() and int(op3[1:]) < 64 and int(op3[1:]) >= 0:
        op3 = int(op3[1:])
        b = to_bin(11 + 21 * condition, 6) + to_bin(op1, 6) + to_bin(op2, 6) + to_bin(op3, 6)

    else:
        error(f"Line {line+1}: Invalid operands")
        return None

    b1 = int(b[:8], 2)
    b2 = int(b[8:16], 2)
    b3 = int(b[16:], 2)
    return bytes([b1, b2, b3])

def cshr(operands, line, condition):
    try:
        op1, op2, op3 = getr1r2x3(operands, line)
    except ValueError:
        return None

    if op3.startswith("R") and op3[1:].isdigit() and int(op3[1:]) < 64 and int(op3[1:]) >= 0:
        op3 = int(op3[1:])
        b = to_bin(13 + 21 * condition, 6) + to_bin(op1, 6) + to_bin(op2, 6) + to_bin(op3, 6)
        
    elif op3.startswith("#") and op3[1:].isdigit() and int(op3[1:]) < 64 and int(op3[1:]) >= 0:
        op3 = int(op3[1:])
        b = to_bin(11 + 21 * condition, 6) + to_bin(op1, 6) + to_bin(op2, 6) + to_bin(op3 + 8, 6)

    else:
        error(f"Line {line+1}: Invalid operands")
        return None

    b1 = int(b[:8], 2)
    b2 = int(b[8:16], 2)
    b3 = int(b[16:], 2)
    return bytes([b1, b2, b3])

def ccmp(operands, line, condition, cc):
    if len(operands) != 2:
        error(f"Line {line+1}: Invalid number of operands")
        return None

    op1, op2 = operands
    op1 = op1.strip()
    op2 = op2.strip()

    if op1.startswith("R") and op1[1:].isdigit() and int(op1[1:]) < 64 and int(op1[1:]) >= 0:
        if op2.startswith("R") and op2[1:].isdigit() and int(op2[1:]) < 64 and int(op2[1:]) >= 0:
            b = to_bin(4 + 21 * condition, 6) + to_bin(cc, 6) + to_bin(int(op1[1:]), 6) + to_bin(int(op2[1:]), 6)
        elif op2.startswith("#") and op2[1:].isdigit() and int(op2[1:]) < 64 and int(op2[1:]) >= 0:
            b = to_bin(4 + 21 * condition, 6) + to_bin(16 + cc, 6) + to_bin(int(op1[1:]), 6) + to_bin(int(op2[1:]), 6)
        else:
            error(f"Line {line+1}: Invalid operands")
            return None
    elif op1.startswith("#") and op1[1:].isdigit() and int(op1[1:]) < 64 and int(op1[1:]) >= 0:
        if op2.startswith("R") and op2[1:].isdigit() and int(op2[1:]) < 64 and int(op2[1:]) >= 0:
            b = to_bin(4 + 21 * condition, 6) + to_bin(24 + cc, 6) + to_bin(int(op1[1:]), 6) + to_bin(int(op2[1:]), 6)
        else:
            error(f"Line {line+1}: Invalid operands")
            return None

    b1 = int(b[:8], 2)
    b2 = int(b[8:16], 2)
    b3 = int(b[16:], 2)
    return bytes([b1, b2, b3])

def ccmpeq(operands, line, condition):
    return ccmp(operands, line, condition, 2)

def ccmpneq(operands, line, condition):
    return ccmp(operands, line, condition, 3)

def ccmpslt(operands, line, condition):
    return ccmp(operands, line, condition, 4)

def ccmpsgt(operands, line, condition):
    return ccmp(operands, line, condition, 5)

def ccmpult(operands, line, condition):
    return ccmp(operands, line, condition, 6)

def ccmpugt(operands, line, condition):
    return ccmp(operands, line, condition, 7)

def cst(operands, line, condition):
    if len(operands) != 2:
        error(f"Line {line+1}: Invalid number of operands")
        return None

    op1, op2 = operands
    op1, op2 = op1.strip(), op2.strip()

    if not op1.startswith("[") or not op1.endswith("]"):
        error(f"Line {line+1}: Invalid operands")
        return None

    op1 = op1.split("+")
    if len(op1) != 2:
        error(f"Line {line+1}: Invalid operands")
        return None

    r1, i1 = op1
    r1, i1 = r1[1:], i1[:-1]

    if not r1.startswith("R") or not r1[1:].isdigit() or not int(r1[1:]) < 64 or not int(r1[1:]) >= 0:
        error(f"Line {line+1}: Invalid operands")
        return None
    else:
        r1 = int(r1[1:])

    if not i1.startswith("#") or not i1[1:].isdigit() or not int(i1[1:]) < 64 or not int(i1[1:]) >= 0:
        error(f"Line {line+1}: Invalid operands")
        return None
    else:
        i1 = int(i1[1:])

    if not op2.startswith("R") or not op2[1:].isdigit() or not int(op2[1:]) < 64 or not int(op2[1:]) >= 0:
        error(f"Line {line+1}: Invalid operands")
        return None
    else:
        op2 = int(op2[1:])
    
    b = to_bin(15 + 21 * condition, 6) + to_bin(op2, 6) + to_bin(r1, 6) + to_bin(i1, 6)
    b1 = int(b[:8], 2)
    b2 = int(b[8:16], 2)
    b3 = int(b[16:], 2)
    return bytes([b1, b2, b3])

def cld(operands, line, condition):
    if len(operands) != 2:
        error(f"Line {line+1}: Invalid number of operands")
        return None

    op1, op2 = operands
    op1, op2 = op1.strip(), op2.strip()

    if not op2.startswith("[") or not op2.endswith("]"):
        error(f"Line {line+1}: Invalid operands")
        return None

    op2 = op2.split("+")
    if len(op2) != 2:
        error(f"Line {line+1}: Invalid operands")
        return None

    r1, i1 = op2
    r1, i1 = r1[1:], i1[:-1]

    if not r1.startswith("R") or not r1[1:].isdigit() or not int(r1[1:]) < 64 or not int(r1[1:]) >= 0:
        error(f"Line {line+1}: Invalid operands")
        return None
    else:
        r1 = int(r1[1:])

    if not i1.startswith("#") or not i1[1:].isdigit() or not int(i1[1:]) < 64 or not int(i1[1:]) >= 0:
        error(f"Line {line+1}: Invalid operands")
        return None
    else:
        i1 = int(i1[1:])

    if not op1.startswith("R") or not op1[1:].isdigit() or not int(op1[1:]) < 64 or not int(op1[1:]) >= 0:
        error(f"Line {line+1}: Invalid operands")
        return None
    else:
        op1 = int(op1[1:])
    
    b = to_bin(14 + 21 * condition, 6) + to_bin(op1, 6) + to_bin(r1, 6) + to_bin(i1, 6)
    b1 = int(b[:8], 2)
    b2 = int(b[8:16], 2)
    b3 = int(b[16:], 2)
    return bytes([b1, b2, b3])

def clbl(operands, line, condition):
    if len(operands) != 2:
        error(f"Line {line+1}: Invalid number of operands")
        return None

    op1, op2 = operands
    op1, op2 = op1.strip(), op2.strip()

    if not op1.startswith("#") or not op1[1:].isdigit() or not int(op1[1:]) < 4096 or not int(op1[1:]) >= 0:
        error(f"Line {line+1}: Invalid operands")
        return None
    else:
        op1 = int(op1[1:])

    if not op2.startswith("#") or not op2[1:].isdigit() or not int(op2[1:]) < 64 or not int(op2[1:]) >= 0:
        error(f"Line {line+1}: Invalid operands")
        return None
    else:
        op2 = int(op2[1:])

    b = to_bin(16 + 21 * condition, 6) + to_bin(op1 // 64, 6) + to_bin(op1 % 64, 6) + to_bin(op2, 6)
    b1 = int(b[:8], 2)
    b2 = int(b[8:16], 2)
    b3 = int(b[16:], 2)
    return bytes([b1, b2, b3])

def cjup(operands, line, condition):
    if len(operands) != 2:
        error(f"Line {line+1}: Invalid number of operands")
        return None

    op1, op2 = operands
    op1, op2 = op1.strip(), op2.strip()

    if not op1.startswith("#") or not op1[1:].isdigit() or not int(op1[1:]) < 4096 or not int(op1[1:]) >= 0:
        error(f"Line {line+1}: Invalid operands")
        return None
    else:
        op1 = int(op1[1:])

    if not op2.startswith("R") or not op2[1:].isdigit() or not int(op2[1:]) < 64 or not int(op2[1:]) >= 0:
        error(f"Line {line+1}: Invalid operands")
        return None
    else:
        op2 = int(op2[1:])

    b = to_bin(17 + 21 * condition, 6) + to_bin(op1 // 64, 6) + to_bin(op1 % 64, 6) + to_bin(op2, 6)
    b1 = int(b[:8], 2)
    b2 = int(b[8:16], 2)
    b3 = int(b[16:], 2)
    return bytes([b1, b2, b3])

def cjdn(operands, line, condition):
    if len(operands) != 2:
        error(f"Line {line+1}: Invalid number of operands")
        return None

    op1, op2 = operands
    op1, op2 = op1.strip(), op2.strip()

    if not op1.startswith("#") or not op1[1:].isdigit() or not int(op1[1:]) < 4096 or not int(op1[1:]) >= 0:
        error(f"Line {line+1}: Invalid operands")
        return None
    else:
        op1 = int(op1[1:])

    if not op2.startswith("R") or not op2[1:].isdigit() or not int(op2[1:]) < 64 or not int(op2[1:]) >= 0:
        error(f"Line {line+1}: Invalid operands")
        return None
    else:
        op2 = int(op2[1:])

    b = to_bin(18 + 21 * condition, 6) + to_bin(op1 // 64, 6) + to_bin(op1 % 64, 6) + to_bin(op2, 6)
    b1 = int(b[:8], 2)
    b2 = int(b[8:16], 2)
    b3 = int(b[16:], 2)
    return bytes([b1, b2, b3])

def cio(operands, line, condition):
    if len(operands) != 3:
        error(f"Line {line+1}: Invalid number of operands")
        return None

    op1, op2, op3 = operands
    op1, op2, op3 = op1.strip(), op2.strip(), op3.strip()

    if not op1.startswith("R") or not op1[1:].isdigit() or not int(op1[1:]) < 4096 or not int(op1[1:]) >= 0:
        error(f"Line {line+1}: Invalid operands")
        return None
    else:
        op1 = int(op1[1:])

    if not op2.startswith("&") or not op2[1:].isdigit() or not int(op2[1:]) < 64 or not int(op2[1:]) >= 0:
        error(f"Line {line+1}: Invalid operands")
        return None
    else:
        op2 = int(op2[1:])

    if not op3.startswith("R") or not op3[1:].isdigit() or not int(op3[1:]) < 4096 or not int(op3[1:]) >= 0:
        error(f"Line {line+1}: Invalid operands")
        return None
    else:
        op3 = int(op3[1:])

    b = to_bin(19 + 21 * condition, 6) + to_bin(op1, 6) + to_bin(op2, 6) + to_bin(op3, 6)
    b1 = int(b[:8], 2)
    b2 = int(b[8:16], 2)
    b3 = int(b[16:], 2)
    return bytes([b1, b2, b3])

CMDS = {
    "ADD": cadd,
    "SUB": csub,
    "OR": cor,
    "XOR": cxor,
    "AND": cand,
    "SHL": cshl,
    "SHR": cshr,
    "CMPEQ": ccmpeq,
    "CMPNEQ": ccmpneq,
    "CMPSL": ccmpslt,
    "CMPSG": ccmpsgt,
    "CMPUL": ccmpult,
    "CMPUG": ccmpugt,
    "LD": cld,
    "ST": cst,
    "LBL": clbl,
    "JUP": cjup,
    "JDN": cjdn,
    "IO": cio,
    "HALT": chalt
}

def assemble(code):
    machine_code = b""
    lines = code.upper().split("\n")
    for line in range(len(lines)):
        text = lines[line].split(";")[0].strip()
        if isempty(text):
            continue
        if " " not in text:
            cmd_end = len(text)
        else:
            cmd_end = text.index(" ")
        cmd = text[:cmd_end]
        if cmd.startswith("+"):
            condition = 1
            cmd = cmd[1:]
        elif cmd.startswith("-"):
            condition = 2
            cmd = cmd[1:]
        else:
            condition = 0
        operands = text[cmd_end:].strip().split(",")
        if cmd in CMDS:
            result = CMDS[cmd](operands, line, condition)
            if result is None:
                return None
            machine_code += result
        else:
            error(f"Line {line+1}: Invalid instruction")
            return None
    return machine_code

def main(filein, fileout):
    with open(filein, "r") as f:
        content = f.read()
    machine_code = assemble(content) 
    if machine_code is not None:
        with open(fileout, "w") as fileout:
            fileout.write(base64.b64encode(machine_code).decode()) 

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} [FILE] [OUTPUT]")
    else:
        main(*sys.argv[1:])