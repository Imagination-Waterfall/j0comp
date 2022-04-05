public class syn2 {

	public int a;
	public int[] b;

	public syn2(int c){
		a = c;
		b = new int[20];
	}

	public static void main(String args[]){
		syn2 myObject;

		myObject = new syn2(23);

		for(int i = 0; i < 20; i++){
			myObject.b[i] = i;
		}
		printb();
	}

	public static void printb(syn2 myObject){
		for(int i = 0; i < 20; i++){
			System.out.println(myObject.b[i]);
		}
	}
}
