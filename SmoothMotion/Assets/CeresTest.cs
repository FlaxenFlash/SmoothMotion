using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;
public class CeresTest : MonoBehaviour {

    [DllImport("CeresWrapper")]

    private static extern void ReturnTest(int[] numbers);

    // Use this for initialization
    void Start () 
    {
        int[] numbers = new int[5];
        var position = transform.position;
        ReturnTest(numbers);
        position.x = numbers[0];
        transform.position = position;
    }
	
    // Update is called once per frame
    void Update () {
	
    }
}
