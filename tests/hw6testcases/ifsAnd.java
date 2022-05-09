public class ifsAnd{
	public static void main(){
		int a;
		int b;
		a = 23;
		b = 0;
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
