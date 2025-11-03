

module GcdCalculator64bit(
    input logic clock,
    input logic reset,
    
    // Avalon-MM slave interface for accessing control-status registers
    input logic csr_read,
    input logic csr_write,
    input logic [2:0] csr_address,
    output logic [31:0] csr_readdata,
    input logic [31:0] csr_writedata
);


    // Control-status registers:
    //     aReg: operand A
    //     bReg: operand B
    //     sReg: status register (contains 0 when computation is done)
    
    logic [31:0] sReg;
    logic [63:0] aReg, bReg;

    always_ff @(posedge clock)
    begin
        if (reset) begin
            sReg <= 1'b0;
            aReg <= 64'b0;
            bReg <= 64'b0;
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
                    // Move data to aReg
                    3'b001: aReg[31:0] <= csr_writedata;
                    3'b010: aReg[63:32] <= csr_writedata;
                    
                    // Move data to bReg
                    3'b011: bReg[31:0] <= csr_writedata;
                    3'b100: begin
                        bReg[63:32] <= csr_writedata;
                        
                        // Writing to the higher part of 
                        // bReg starts the computation process
                        sReg <= 1'b1; 
                    end
                endcase
            end
            
            // Read is always possible
            if (csr_read) begin
                case(csr_address)
                    3'b000: csr_readdata <= sReg;
                    3'b001: csr_readdata <= aReg[31:0];
                    3'b010: csr_readdata <= aReg[63:32];
                    3'b011: csr_readdata <= bReg[31:0];
                    3'b100: csr_readdata <= bReg[63:32];
                endcase
            end
        end
    end
    
endmodule