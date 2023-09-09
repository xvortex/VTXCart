`timescale 1ns/1ns

// MVS CHA CPLD (CP1)
// 2023 Vortex

module cha_top(

	input [22:0] PBUS,
	
	input  [1:0] SDA_L,
	input [15:8] SDA_U,
	
	input CA4, S2H1,
	input PCK1B, PCK2B,
	input SDRD0, SDROM, SDMRD,

	output [26:0] C_ADDR,

	output  [1:0] C1_nOE,
	output  [1:0] C2_nOE,

	output [16:0] S_ADDR,

	output [17:11] M_ADDR,

	output M1_nCE, M1_nOE,

	input   [7:0] GSEL
);

// 28..........................0
// xxxxxxxxxxxxxxxxxxxxxxxxxxxxx - C_ADDR_ALL
// x............................ - C1/C2
// .x........................... - ??nOE
// ..xxxxxxxxxxxxxxxxxxxxxxxxxxx - C_ADDR (for 1 F0095H0)
// .....xxxxxxxxxxxxxxxxxxxxxxxx - C_ADDR_1 (for 1 game)
// xxxxxxxxxxx.................. - IX[GSEL]

//    25                       0
//    0GGGGGGGGxxxxxxxxxxxxxxxxx - S_ADDR (for JS28F512, G=GSEL by hw.)

//    25                       0
//    GGGGGGGGxxxxxxxSSSSSSSSSSS - M_ADDR (for JS28F512, G=GSEL by hw., S=SDA by hw.)

	assign C_ADDR = C_ADDR_ALL[26:0];

	wire [28:0] C_ADDR_ALL = {IX + C_ADDR_1[23:18], C_ADDR_1[17:0]};

	reg [5:0] MASK;
	reg [10:0] IX;

	wire [23:0] C_ADDR_1 = {C_LATCH_U[2:0] & MASK[5:3], C_LATCH[19:17] & MASK[2:0], C_LATCH[16:4], CA4, C_LATCH[3:0]};

	reg [2:0] C_LATCH_U;
	wire [19:0] C_LATCH; // assigned in NEO-273

	wire [15:0] S_LATCH; // assigned in NEO-273
	assign S_ADDR = {S_LATCH[15:3], S2H1, S_LATCH[2:0]};

	wire [18:11] MA;     // assigned in NEO-ZMC
	assign M_ADDR = MA[17:11];

	// M_CE#, M_OE#
	assign M1_nCE = SDROM;
	assign M1_nOE = SDMRD;

	// C_OE#
	assign C1_nOE = (C_ADDR_ALL[28]) ? 2'b11 : {~C_ADDR_ALL[27], C_ADDR_ALL[27]};
	assign C2_nOE = (C_ADDR_ALL[28]) ? {~C_ADDR_ALL[27], C_ADDR_ALL[27]} : 2'b11;

	// Pseudo reset
	wire nRESET = (GSEL == 8'd0) ? 1'b0 : 1'b1;

	// upper addr latches
	always @(posedge PCK1B)
	begin
		C_LATCH_U <= PBUS[22:20];
	end

	// NEO-273
	neo_273 NEO_273(PBUS[19:0], PCK1B, PCK2B, C_LATCH, S_LATCH);
	
	// NEO-ZMC
	zmc ZMC(nRESET, SDRD0, SDA_L[1:0], SDA_U[15:8], MA);

	// IX[GSEL]
	always @(*)
	begin
		case (GSEL)
			`include "ix_c.inc"
			default: begin MASK <= 6'b111111; IX <= 11'b00000000000; end
		endcase
	end

endmodule
