import requests
import bs4

def get_instruction_table(cb=False):
    source = requests.get("https://gbdev.io/gb-opcodes/optables/").text
    soup = bs4.BeautifulSoup(source, features="html5lib")

    table = []

    for n in range(0xff + 1):
        if not cb:
            td = soup.find("td", {"id": "op-" + format(n, "02x")})
        else:
            td = soup.find("td", {"id": "op-cb" + format(n, "02x")})

        instruction = td.find("span", {"class": "mnemonic"})
        if instruction is None:
            mnemonic = "ILLEGAL"
        else:
            mnemonic = instruction.text.strip()

        argType = "NoImmediate"
        if "n8" in mnemonic:
            mnemonic = mnemonic.replace("n8", "0x%02X")
            argType = "Immediate8"
        elif "n16" in mnemonic:
            mnemonic = mnemonic.replace("n16", "0x%04X")
            argType = "Immediate16"
        elif "a8" in mnemonic:
            mnemonic = mnemonic.replace("a8", "%02X")
            argType = "Immediate8"
        elif "a16" in mnemonic:
            mnemonic = mnemonic.replace("a16", "%04X")
            argType = "Immediate16"
        elif "e8" in mnemonic:
            mnemonic = mnemonic.replace("e8", "%02X")
            argType = "Immediate8"

        table.append((mnemonic, argType))

    return table

def table_to_c(vname, table):
    c_code = "const std::string " + vname + "[] = {\n"
    for i in range(16):
        c_code += "\t"
        for j in range(16):
            c_code = c_code + "\"" + table[16 * i + j] + "\"" + ", "
        c_code += "\n"
    c_code += "};"
    return c_code

def table_to_c2(vname, table):
    c_code = "const AbstractInstruction " + vname + "[] = {\n"
    for i in range(0xff + 1):
        c_code += "\t{"
        c_code += "\"{}\", {}".format(
            table[i][0], table[i][1])
        c_code += "},\n"
    c_code += "};"
    return c_code

print(table_to_c2("InstructionTable", get_instruction_table()))
print()
print(table_to_c2("InstructionTableCB", get_instruction_table(True)))