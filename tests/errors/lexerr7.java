public class lex1 {
	//this is a line comment
	/*
		this is a block
		comment
	*/
	public int attribute = 5;
	public static void main(String[] args){
		boolean TF = true;
		long z = 10l;
		TF = false;
		char x = '\n';
		String y = "This is a test";
		int num = {10, 20, 23, 34};
		String newString = new String("New String");

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
			switch(TF`){//err
				case true:
					TF = false;
					break;
				case false:
					TF = true;
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
	}

	public static int retNull(){
		return NULL;
	}
}
