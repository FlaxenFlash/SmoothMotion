using UnityEngine;
using System;
using System.Collections.Generic;
using System.IO;

public class SmoothMotionAnimator : MonoBehaviour 
{
    public SmoothMotionController Controller;
    private List<Vector3> _bonePositions;
    private float _frame;

	void Start () 
    {
        _frame = 0;
        ReadFromFile();
	}

    private void ReadFromFile()
    {
        _bonePositions = new List<Vector3>();

        var content = File.ReadAllText(Controller.AnimationFilename);
        var values = content.Split(new char[] {' '});

        var frames = Int32.Parse(values[2]);
        if (frames != Controller.NumFrames)
        {
            Debug.LogWarning("Loaded file has a different number of frames from currently selected animation, expect the unexpected");
        }

        for(var i = 4; i< values.Length; i+=3)
        {
            _bonePositions.Add(new Vector3(Parse(values[i-2]), Parse(values[i-1]), Parse(values[i])));
        }

        if(_bonePositions.Count != Controller.Bones.Count)
        {
            Debug.LogWarning("Loaded file has a different number of bones from currently selected animation, expect the unexpected");
        }
    }

    private float Parse(string value)
    {
        return float.Parse(value);
    }
	
    void Update()
    {
        _frame += Time.deltaTime * Controller.SamplingFPS;
        foreach (var bone in Controller.Bones)
        {
            bone.SetFrame(_frame);
        }
    }
}
