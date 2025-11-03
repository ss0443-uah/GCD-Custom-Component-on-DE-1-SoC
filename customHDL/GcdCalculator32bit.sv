

module GcdCalculator32bit(
    input logic clock,
    input logic reset,
    
    // Avalon-MM slave interface for accessing control-status registers
    input logic csr_read,
    input logic csr_write,
    input logic [1:0] csr_address,
    output logic [31:0] csr_readdata,
    input logic [31:0] csr_writedata
);


    // Control-status registers:
    //     aReg: operand A
    //     bReg: operand B
    //     sReg: status register (contains 0 when computation is done)
    
    logic [31:0] aReg, bReg, sReg;

    always_ff @(posedge clock)
    begin
        if (reset) begin
            sReg <= 1'b0;
            aReg <= 32'b0;
            bReg <= 32'b0;
        end
        
        else begin
            // While sReg is set, keep doing computations
            if (sReg) begin
                if (aReg > bReg) aReg <= aReg - bReg;
                else if (bReg > aReg) bReg <= bReg - aReg;
                else sReg <= 1'b0;
            end
            
            // Write control-status registers
            // When computation is going, write is disabled
            else if (csr_write) begin
                case(csr_address)
                    2'b01: aReg <= csr_writedata;
                    2'b10: begin
                        bReg <= csr_writedata;
                        sReg <= 1'b1;
                    end
                endcase
            end
            
            // Read is always possible
            if (csr_read) begin
                case(csr_address)
                    2'b00: csr_readdata <= sReg;
                    2'b01: csr_readdata <= aReg;
                    2'b10: csr_readdata <= bReg;
                endcase
            end
        end
    end
    
endmodule