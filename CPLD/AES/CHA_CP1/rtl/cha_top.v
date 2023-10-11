`timescale 1ns/1ns

// AES CHA CPLD (CP1)
// 2023 Vortex

module cha_top(

	input CLK_12M,
	input SDRD0, SDROM, SDMRD,
	input PCK1B,

	input   [31:0] CR,

	output [26:17] C_ADDR,

	output  [1:0] C1_nOE,
	output  [1:0] C2_nOE,
	output  [1:0] C3_nOE,

	input   [15:12] PBUS_L,
	input   [22:20] PBUS_U,

	input  [1:0] SDA_L,
	input [15:8] SDA_U,

	input EVEN, LOAD, H,

	output [3:0] GAD,
	output [3:0] GBD,
	output DOTA, DOTB,

	output [17:11] M_ADDR,

	output M1_nCE, M1_nOE,

	input   [7:0] GSEL
);

// 29..........................0
// xxxxxxxxxxxxxhhhhhhhhhhhhhhhhh - C_ADDR_ALL (h=hw.)
// xx............................ - C1-C4
// ..x........................... - ??nOE
// ...xxxxxxxxxxhhhhhhhhhhhhhhhhh - C_ADDR (for 1 F0095H0)
// ......xxxxxxx................. - C_ADDR_1 (for 1 game)
// xxxxxxxxxxxx.................. - IX[GSEL]

//    25                       0
//    0GGGGGGGGhhhhhhhhhhhhhhhhh - S_ADDR (for JS28F512, G=GSEL by hw., h=hw.)

//    25                       0
//    GGGGGGGGxxxxxxxSSSSSSSSSSS - M_ADDR (for JS28F512, G=GSEL by hw., S=SDA by hw.)

	assign C_ADDR [26:17] = C_ADDR_ALL[26:17];

	wire [29:17] C_ADDR_ALL = {IX + C_ADDR_1[23:18], C_ADDR_1[17]};

	reg [5:0] MASK;
	reg [11:0] IX;

	wire [23:17] C_ADDR_1 = {C_LATCH_U[23:21] & MASK[5:3], C_LATCH_L[20:18] & MASK[2:0], C_LATCH_L[17]};

	reg [23:21] C_LATCH_U;
	reg [20:17] C_LATCH_L;

	wire [18:11] MA;     // assigned in NEO-ZMC
	assign M_ADDR = MA[17:11];

	// M_CE#, M_OE#
	assign M1_nCE = SDROM;
	assign M1_nOE = SDMRD;

	// C_OE#
	assign C1_nOE = (C_ADDR_ALL[29:28] == 2'b00) ? {~C_ADDR_ALL[27], C_ADDR_ALL[27]} : 2'b11;
	assign C2_nOE = (C_ADDR_ALL[29:28] == 2'b01) ? {~C_ADDR_ALL[27], C_ADDR_ALL[27]} : 2'b11;
	assign C3_nOE = (C_ADDR_ALL[29:28] == 2'b10) ? {~C_ADDR_ALL[27], C_ADDR_ALL[27]} : 2'b11;

	// Pseudo reset
	wire nRESET = (GSEL == 8'd0) ? 1'b0 : 1'b1;

	// addr latches
	always @(posedge PCK1B)
	begin
		C_LATCH_L[20:17] <= PBUS_L[15:12];
		C_LATCH_U[23:21] <= PBUS_U[22:20];
	end

	// NEO-ZMC
	zmc ZMC(nRESET, SDRD0, SDA_L[1:0], SDA_U[15:8], MA);

	// NEO-ZMC2
	zmc2_dot ZMC2(CLK_12M, EVEN, LOAD, H, CR, GAD, GBD, DOTA, DOTB);

	// IX[GSEL]
	always @(*)
	begin
		case (GSEL)
			`include "ix_c.inc"
			default: begin MASK <= 6'b111111; IX <= 12'b000000000000; end
		endcase
	end

endmodule
