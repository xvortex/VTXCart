`timescale 1ns/1ns

// 2020 Sean 'furrtek' Gonsalves
// 2023 Vortex

module pcm(
	input MODE, // 0=VROM+PCM, 1=VROMA/VROMB
	input CLK_68KCLKB,
	input nSDROE, SDRMPX,
	input nSDPOE, SDPMPX,
	inout [7:0] SDRAD,
	input [9:8] SDRA_L,
	input [23:20] SDRA_U,
	inout [7:0] SDPAD,
	input [11:8] SDPA,
	input [7:0] D,
	output [23:0] A
);

	reg [19:0] RALATCH;
	reg [23:0] PALATCH;
	reg [4:0] SR;
	reg [7:0] D_LATCH;
	wire [7:0] RD_LATCH;

	always @(posedge SDRMPX)
	   RALATCH[9:0] <= {SDRA_L, SDRAD};
	always @(negedge SDRMPX)
	   RALATCH[19:10] <= {SDRA_L, SDRAD};

	always @(posedge SDPMPX)
	   PALATCH[11:0] <= {SDPA, SDPAD};
	always @(negedge SDPMPX)
	   PALATCH[23:12] <= {SDPA, SDPAD};

	assign A = nSDPOE ? {SDRA_U, RALATCH} : PALATCH | (MODE ? 24'h200000 : 24'h0);

	wire TRIGTEST, C49B, C68B;

	assign TRIGTEST = SR[4];

	assign C49B = ~(TRIGTEST | nSDPOE);
	assign C68B = ~(C49B | nSDROE);

	// A37 C49A
	always @(posedge CLK_68KCLKB or negedge C68B)
	begin
		if (!C68B)
			SR <= 5'b00000;
		else
			SR <= {SR[3:0], 1'b1};
	end

   	always @(posedge TRIGTEST)
       	D_LATCH <= D;

	assign RD_LATCH = TRIGTEST ? D_LATCH : D;
	assign SDRAD = nSDROE ? 8'bzzzzzzzz : RD_LATCH;
	assign SDPAD = nSDPOE ? 8'bzzzzzzzz : D;

endmodule
