`timescale 10ns/1ns
	module tb_74X138(
		
	);
	reg [2:0] A;
	reg E3,E2,E1;
	wire [7:0] Y;

	_74X138 U1(A,E3,E2,E1,Y);//元件实例化
	initial 
		$monitor($time,": A = %b, E3 = %b, E2 = %b, E1 = %b", A, E3, E2, E1);//监视器监视。
	initial
	begin
		/* 控制信号无效 */
		E3 = 0;
		E2 = 0;
		E1 = 0;
		A = 3'b000;
		#5                     
		E3 = 1;
		E2 = 1;
		E1 = 0;
		A = 3'b000;
		#5
		
		/* 控制信号有效 */
		E3 = 1;
		E2 = 0;
		E1 = 0;
		A = 3'b000;
		#5
		E3=1;E2=0;E1=0;A=3'b010;
		#5
		E3=1;E2=0;E1=0;A=3'b111;
		#5
		
		/* 停止仿真 */
		$stop;
	end
endmodule
