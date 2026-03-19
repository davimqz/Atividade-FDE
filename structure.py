"""
######################################
# Esqueleto do processador em Pythão #
######################################
"""
class MiniCPU:
    def __init__(self):
        self.mem = [0] * 256
        self.reg = [0, 0, 0, 0]  # R0-R3
        self.pc = 0
        self.zf = 0
        self.running = True
        self.ciclo = 0

    def fetch(self):
        op = self.mem[self.pc]
        a  = self.mem[self.pc + 1]
        b  = self.mem[self.pc + 2]
        self.pc += 3
        return op, a, b

    def decode_execute(self, op, a, b):
        if   op == 0x01: self.reg[a] = self.mem[b]          # LOAD
        elif op == 0x02: self.mem[b] = self.reg[a]          # STORE
        elif op == 0x03:                                    # ADD
            self.reg[a] = (self.reg[a] + self.reg[b]) & 0xFF
        elif op == 0x04:                                    # SUB
            self.reg[a] = (self.reg[a] - self.reg[b]) & 0xFF
        elif op == 0x05: self.reg[a] = b                    # MOV
        elif op == 0x06:                                    # CMP
            self.zf = 1 if self.reg[a] == self.reg[b] else 0
        elif op == 0x07: self.pc = a                        # JMP
        elif op == 0x08:                                    # JZ
            if self.zf: self.pc = a
        elif op == 0x09:                                    # JNZ
            if not self.zf: self.pc = a
        elif op == 0x0A: self.running = False               # HALT

    def trace(self, op, a, b):
        nomes = {1:'LOAD',2:'STORE',3:'ADD',4:'SUB',
                 5:'MOV',6:'CMP',7:'JMP',8:'JZ',9:'JNZ',10:'HALT'}
        nome = nomes.get(op, '???')
        print(f'Ciclo {self.ciclo}: {nome:5s} {a},{b} |'
              f' R0={self.reg[0]:3d} R1={self.reg[1]:3d}'
              f' R2={self.reg[2]:3d} R3={self.reg[3]:3d}'
              f' | PC={self.pc:3d} ZF={self.zf}')

    def run(self):
        while self.running and self.pc < 256:
            self.ciclo += 1
            op, a, b = self.fetch()
            self.decode_execute(op, a, b)
            self.trace(op, a, b)
