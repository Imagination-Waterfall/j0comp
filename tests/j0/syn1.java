public class syn1 {
	//this is a line comment
	/*
		this is a block
		comment
	*/
	public int attribute = 5;

	public static void main(String[] args){
		int intArray[];
		int[] intArrayTwo;
		char x;
		boolean TF = true;
		long z = 10l;
		TF = false;
		x = '\n';
		String y = "This is a test";
		int[] nums = {10, 20, 23, 34};
		int numstwo[] = {10, 20, 23, 34};

		String newString = new String("New String");

		intArray = new int[20];
		intArrayTwo = new int[]{1,2,3,4};

		attribute = intArrayTwo[3];

		x = 'a';
		x = '\t';
		x = '\"';
		x = '\'';
		x = '';

		z = z + 10 - 13 * 34 / 2 % 13;
		z += 3;
		z -= 1;
		z++;
		z--;

		for(attribute = 10; attribute < 21; attribute++){
			if(TF){
				break;
			}
		}

		if(attribute > 5){
			attribute = 5;
		}else if (attribute <= 15){
			attribute = 14;
		}else if (attribute >= 12){
			attribute = 11;
		}else if (attribute > 13){
			attribute = 13;
		}

		if (z == 23 && attribute != 23){
			System.out.println(y);
		}else if (z == 2 || !TF){
			System.out.println("nonsense");
		}

		while(TF){
			TF = !TF;
		}

		printThing("Thing that is printed");
	}

	public static void printThing(String thing){
		System.out.println(thing);
	}
	public static String retNull(){
		return NULL;
	}
}
