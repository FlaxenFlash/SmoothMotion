using UnityEngine;
using System;
using System.Collections.Generic;
using System.IO;

public class SmoothMotionAnimator : MonoBehaviour 
{
    public SmoothMotionController Controller;
    private float _frame;
    private const string fileExt = ".SMOutput";

	private bool _initialised;
	public float SlowMotion = 1;

	void Start()
    {
        _frame = 0;
	}

    private void ReadFromFile()
    {
        var bonePositions = new List<Vector3>();

        var content = File.ReadAllText(Controller.AnimationFilename+fileExt);
        var values = content.Split(' ');

        var frames = Int32.Parse(values[0]);
        if (frames != Controller.NumFrames)
        {
            Debug.LogWarning("Loaded file has a different number of frames from currently selected animation, aborting");
            return;
        }

        for(var i = 3; i< values.Length; i+=3)
        {
            bonePositions.Add(new Vector3(Parse(values[i-2]), Parse(values[i-1]), Parse(values[i])));
        }
        
        if(bonePositions.Count/frames != Controller.Bones.Count)
        {
            Debug.LogWarning("Loaded file has a different number of bones from currently selected animation, aborting");
            return;
        }

        int position = 0;

        foreach(var bone in Controller.Bones)
        {
			for(var frame = 0; frame < frames; frame++)
            {
				bone.OverridePosition(bonePositions[position], frame);
				position++;
            }
        }
    }

    private float Parse(string value)
    {
        return (float)double.Parse(value);
    }
	
    void Update()
    {
		if (Controller == null || !Controller.Initialised)
			return;

		if (!_initialised)
		{
			ReadFromFile();
			_initialised = true;
		}

		_frame += Time.deltaTime * Controller.SamplingFPS * SlowMotion;
		foreach (var bone in Controller.Bones)
		{
			bone.SetFrame(_frame);
		}
    }
}
