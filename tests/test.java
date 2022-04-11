public class bar {

	public int a = 10;
	/*public int u[];
	public int []p;
	public int b;*/

	bar(int x){
		a = x;
	}
	public static long[] foo() {
		int a[] = {20};
		return a;
	}
	public static int foobar() {
		int a = 40;
		return a;
	}

	public static int main(int []argv, String args[], int y)
	{
		int z;
		int asdf[];
		int []t = {10, 40, 20};

		z = foobar();
		t = foo();
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
