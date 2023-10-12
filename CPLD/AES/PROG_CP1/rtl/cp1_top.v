`timescale 1ns/1ns

// PROG CPLD (CP1)
// 2023 Vortex

module cp1_top(

	input  [19:1] M68K_ADDR,
	inout  [15:0] M68K_DATA,

	input nPORTWEL, nPORTWEU, nPORTOEL, nPORTOEU,
	input nROMOEL, nROMOEU, nROMOE,
	input nAS, M68K_RW,
	input nRESET, nRESET2,

	output    [25:0] P_ADDR,
	output reg [2:0] P_nCE,
	output           P_nOE,

	inout   [7:0] MCU_DATA,

	output  [9:0] ICON
);

// 27.........................0
// xxxxxxxxxxxxxxxxxxxxxxxxxxxx - P_ADDR_ALL
// xx.......................... - P_nCE
// ..xxxxxxxxxxxxxxxxxxxxxxxxxx - P_ADDR (for 55LV100S)
// .........xxxxxxxxxxxxxxxxxxx - M68K_ADDR
// xxxxxxxxx................... - IX
// ......xxx................... - P2_BANK

	assign M68K_DATA = 16'bzzzzzzzzzzzzzzzz; // always read
	assign MCU_DATA  =  8'bzzzzzzzz;         //

	wire [27:0] P_ADDR_ALL;

	assign P_ADDR = P_ADDR_ALL[25:0];

	wire [1:0] CSEL = P_ADDR_ALL[27:26];

	reg P2_MIRROR;
	reg [2:0] P2_BANK;
	reg [3:0] P2_BASE;
	reg [2:0] BANKS;

	reg [8:0] IX;

	reg [7:0] GSEL;

	assign ICON = {1'bz, GSEL[7:3], 1'bz, GSEL[2:0]};

	wire nPORTOE = nPORTOEL & nPORTOEU;

	assign P_nOE = nROMOE & nPORTOE;

	// P_CE#
	always @(*)
	begin
		case (CSEL)
			2'b00: P_nCE <= 3'b110;
			2'b01: P_nCE <= 3'b101;
			2'b10: P_nCE <= 3'b011;
			2'b11: P_nCE <= 3'b111;
		endcase
	end

	assign P_ADDR_ALL = {IX + ((!nROMOE) ? 9'd0 : {5'd0, P2_BASE}), M68K_ADDR};

	// P2 bank latch
	always @(posedge nPORTWEL or negedge nRESET)
	begin
		if (!nRESET)
		begin
			P2_BANK <= 3'd0;
			GSEL    <= 8'd0;
		end
		else
		begin
			P2_BANK <= (M68K_DATA[2:0] <= BANKS) ? M68K_DATA[2:0] : 3'd0;
			if (M68K_ADDR[19:1] == 19'b1100000011111110111) GSEL <= M68K_DATA[7:0]; // 0x2C0FEE
		end
	end

	// GSEL/P_BASE
	always @(*)
	begin
		case (GSEL)
			`include "ix_p.inc"
			default: begin BANKS <= 3'd0; IX <= 9'b000000000; P2_MIRROR <= 1'b1; end
		endcase

		if(P2_MIRROR)
			P2_BASE <= 4'b0;
		else
			P2_BASE <= {1'b0, P2_BANK } + 1'b1;
	end

endmodule
