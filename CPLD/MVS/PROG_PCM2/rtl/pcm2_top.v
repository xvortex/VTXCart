`timescale 1ns/1ns

// PCM2 CPLD
// 2023 Vortex

module pcm2_top(

	input nSDROE, SDRMPX,
	input [23:20] SDRA_U,
	input [9:8]   SDRA_L,
	
	input nSDPOE, SDPMPX,
	input [11:8]  SDPA,
	
	input CLK_68KCLKB,
	
	inout [7:0]   SDPAD,
	inout [7:0]   SDRAD,

	output [26:0] V_ADDR,
	input  [31:0] V_DATA,
	output [3:0]  V_nOE,

	input [9:0]   ICON
);

// 29...........................0
// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx - V_ADDR_ALL
// .xxxxxxxxxxxxxxxxxxxxxxxxxxx.. - V_ADDR (for F0095H0)
// ............................xx - V_DATA_8 (V_DATA 0123)
// x............................. - V_nOE (1+3/2+4)
// ......xxxxxxxxxxxxxxxxxxxxxxxx - V_ADDR_1 (for 1 game)
// xxxxxxxxx..................... - IX[GSEL]

// xxxxxxxxxx - ICON
// .xxxxx.xxx - GSEL
// x.....x... - NC

	assign V_ADDR = V_ADDR_ALL[28:2];

	wire [29:0] V_ADDR_ALL = {IX + V_ADDR_1[23:21], V_ADDR_1[20:0]};

	reg MODE;
	reg [8:0] IX;

	wire [23:0] V_ADDR_1; // assigned in PCM

	reg [7:0] V_DATA_8;

	always @(*)
	begin
		case (V_ADDR_1[1:0])
			3'b00: V_DATA_8 <= V_DATA[7:0];
			3'b01: V_DATA_8 <= V_DATA[15:8];
			3'b10: V_DATA_8 <= V_DATA[23:16];
			3'b11: V_DATA_8 <= V_DATA[31:24];
		endcase
	end

	// V_OE#
	assign V_nOE = (V_ADDR_ALL[29]) ? 4'b0101 : 4'b1010;

	// PCM
	pcm PCM(MODE, CLK_68KCLKB, nSDROE, SDRMPX, nSDPOE, SDPMPX, SDRAD, SDRA_L, SDRA_U, SDPAD, SDPA, V_DATA_8, V_ADDR_1);

	wire [7:0] GSEL = {ICON[8:4], ICON[2:0]};

	// GSEL
	always @(*)
	begin
		case (GSEL)
			`include "ix_v.inc"
			default: begin MODE <= 1'b0; IX <= 9'b000000000; end
		endcase
	end

endmodule
