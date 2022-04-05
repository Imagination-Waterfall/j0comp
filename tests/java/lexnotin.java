import java.util.Scanner; //not Supported

package javalex; //package not supported

public class lex1 {
	//this is a line comment
	/*
		this is a block
		comment
	*/
	private int y = 4; //private not supported
	public int attribute = 5;
	protected String fname = "Not Supported in j0";

	public static void main(String[] args) throws ArithmeticException{ // throws not supported
		short num = 3; //not supported
		double doub = 45.234; // double not Supported
		float floa = 12.3412; //float not supoorted
		boolean TF = true;
		long z = 10l;
		TF = false;
		char x = '\n';
		String y = "This is a test";
		int num = {10, 20, 23, 34};
		String newString = new String("New String");

		enum is{
			not,
			included,
			in,
			j0
		}

		System.out.println(newString instanceof String); //instance of not supported

		assert attribute = 6 : "\'assert\' not included in j0";
		byte b = 55; // byte not in j0


		x = 'a';
		x = '\t';
		x = '\"';
		x = '\'';
		x = '';

		try{
			z = z + 10 - 13 * 34 / 2 % 13;
			z += 3;
			z -= 1;
			z++;
			z--;
		}catch(Exception e){
			throw new ArithmeticException("try/catch not in j0");
		} finally {
			System.out.println("finally is also not in j0");
		}

		do{ //do not in j0
			z++;
			if(z == 34){
				continue;
			}
			System.out.println("continue not supported" + z);
		}while(z != 23412345);

		for(attribute = 10; attribute < 21; attribute++){
			switch(TF){ //switch case not supported in j0
				case true:
					TF = false;
					break;
				case false:
					TF = true;
					break;
				default:
					System.out.println("default - If you see this message then something is increadibly wrong.");
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
public abstract class abs {
	//abstract not in j0

	final int isnotinj0 = 10;
}
public class notInJ0 extends abs{

}

interface Animal{
	//is not in j0
}

public class Pig implements Animal{
	//also not in j0
}

public native void test(){
	//not supported in j0
}

public class parent{
	public int age;

	public parent(){
		this.age = 100000; // this not supported
	}
	public static void YellAtKids(){
		System.out.println("JEFFRY GET IN HERE NOW");
	}
}

public class mom extends parent{
	public static void YellAtKids(){
		super.YellAtKids(); //super not supported
		System.out.println("It was at this moment that he knew... he f***ed up");
	}
}
