public class foo {
	public static double c;
	//show that functions have their own scopes
	public static int bar(int x){
		int quack;
		quack = 34 * x;
		return quack;
	}
	public static void main(String args, int argv){
		int a;
		//assignment initializer
		int b = 1;

		//math
		a = 1;
		a = a + 1;
		a = a - 1;
		a = a * b;
		b = a / 10;
		b = a % 23;
		a = -b;

		//all together
		a = 10 + b - 2 * b / 34 % 2;

		//if statement
		// EqExpr
		if(a == 10){
			b = a;
		}
		//Not Eq
		if(b != a){
			b = a;
		}

		//RelExpr Ifs
		if(b > a){
			//more math that works
			a++;
		}

		if(b < a){
			b++;
		}

		if(b >= a){
			a++;
		}

		if(b <= a){
			b++;
		}

		//andexpr
		if(a > b && b < 3){
			a = b - 3;
		}

		//nested andexpr
		if(a > b && b < 3 && a != 2 && b == 1){
			a--;
		}

		//one last thing to show that label is placed correctly
		int c = a * b / 3;
	}
}
