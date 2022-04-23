/*public class basic {
	public static int x ;
	public static void main(String []argv) {
	   x = 5;
	   x = x * x;
	}
}*/

/*public class list {
	public static void main(String []argv) {
	   int [] a;
	   a = new int [8];
	   a[5] = a[4];
	   System.out.println(""+a[5]);
	}
}*/

/*public class undeclared {
	public static void main(String []argv) {
	   System.out.println(a);
	}
}*/

/*public class redeclared {

	public static int n1 ;
	public static int n2 ;

	public static void f(double n1) {
	   double n2;
	   int y;

	   double n1;
	   System.out.println(""+ n1);
	   return;
	}
}*/

//public class test{
	/*public static long []threelen(long []a){
		long []ret = a;
		return ret;
	}
	public static double plusten(float []x, char y){
		double ret = x[3] + y + 10;
		return ret;
	}*/
	//public static void main(){
		//String x = "this is a test";
		//int []b = new int[12];
		/*boolean a;
		double x;
		int []b = {34, 54, 12};
		long []y = threelen(b);
		x = plusten(threelen(b), '3');*/
		//Array.set(new int[12], new int[3] , new int[3]);
		//b.println();
		/*int a = 4;
		int b;
		b = 3;
		if(a == 3){
			int c = 3;
		}
		if(a == 3){
			int sa = 3;
		}else{
			b = 3;
		}

		if((a = b) == 3){
			b = a;
		}else if(a == b){
			a = 0;
			b = 0;
		}else{
			a = b;
		}*/
		/*int a = 3;
		float b = 34.3;
		while(b == a){
			a++;
		}*/
		/*int i, a, b, c, d;
		for(i = 45; i = 3; i++){

		}
	}
}*/

public class bar {

	public static int a;
	public static int b = 10;
	public static int []c;
	public static int d[];
	public static int []e = new int[1];
	public static int f[] = {10, 30};
	public static boolean y = a + 10 - 13 * 34 / 2 % 13 > b;
	public static int []x = new int[10];
	public static int w[] = new int[10];
	public static int u[];
	public static int []p;

	public static void test(int a, long b[], String c, char d, boolean e){

	}
	public static int[] test2(){
		return new int[12];
	}
	/*public static int[] ab() {
		//int a[] = {20};
		return new int[12];
	}*/
	/*public static int[] aa() {
		int a[] = {20};
		return a;
	}

	public static String ac() {
		int a[] = {20};
		return "test";
	}*/

	/*public static String ad() {
		int a[] = {20};
		return new String("test");
	}*/
	/*public static String[] ae() {
		int a[] = {20};
		return new String[10];
	}

	public static int[] af() {
		//int a[] = {20};
		return ae();
	}*/
	//public static int d;
	/*public static int[] foo(int z, long y){

	}*/
	public static int main()
	{
		//bar b = new bar();
		//String a = new String("String");
		long c;
		int d[] = {10, 23};
		int []f;
		int []x = new int[10];
		boolean e = false;
		f = test2();
		test(f[3], x, "", 'd', false);

		a = b + 10;
		b = a - 13;
		a = b * 34;
		b = a / 2;
		a = b % 13;
		int z = 2;
		z = z + 10 - 13 * 34 / 2 % 13;
		int y = z + 10 - 13 * 34 / 2 % 13;
		if(a == b){
			boolean l = !false;
			boolean i = e && l;
			boolean j = i || l;
			boolean m = a < b;
			boolean o = a > b;

		}

		/*//bar c; constructor tests*/
		/*int a = 10;
		int b;
		b = a;*/
		//String a = new String("a");
		//a = 10;
		//Test for regular assignments
		/*int a;
		long b;
		boolean c;
		String d;
		char e;
		b = 10l;
		c = true;
		d = "This is a test\n";
		e = 't';*/

		/*
		//Test for function calls
		foobar(a);
		*/


		//Test for assignmnet function calls
		//int d = 10;
		//a = foobar(10);
		//int b = foobar(a);
		//int e[] = foo();

		//tests for Arrays
		/*int c[];
		int e[] = new int[10];
		int []f = new int[10];
		long g[] = {11l, 12l};
		int []d;
		int []h = {21, 32, 45};
		c = new int[10];
		d = {};
		d = {23, 45, 56, 78, 234, 54, 67, 23};*/

		//Test for assignment declarators
		/*int f = 10;
		long g = 23l;
		boolean h = true;
		String i = "This is a test \n";
		char j = 't';*/


		/*int z;
		int asdf[];
		int []t = {10, 40, 20};
		t={};

		z = foobar(34);
		t = foo();

		t[0] = foobar(z);*/
		/*int []i;
		int []b = new int[10];
		int c[] = new int[10];
		String tsnt[] = new String[5];
		z = foo(5, "funf");

		for(int v = t[1]; i < 20; i++){

		}
		for(z = t[1]; z < 20; z++){

		}*/

		return 0;
	}
}
