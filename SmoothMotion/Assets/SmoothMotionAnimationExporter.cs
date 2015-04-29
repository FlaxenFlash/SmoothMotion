using System.Collections.Generic;
using UnityEngine;
using System.IO;


public class SmoothMotionAnimationExporter : MonoBehaviour
{
    public SmoothMotionController Controller;
    public List<PlaneCollisionConstraint> Constraints;
    private const string fileExt = ".SMInput";
	private bool _initialised;

    void Update()
    {
		if (Controller == null || !Controller.Initialised)
			return;

		if (!_initialised)
		{
			WriteToFile();
			_initialised = true;
		}
    }

    private void WriteToFile()
    {
        var output = "";

        //File format: NumBones, NumFrames, NumColliders, Bone Positions(All frames for each bone), Collider Details (Position, Normal)
        output += Controller.Bones.Count + " " + Controller.NumFrames + " " + Constraints.Count + " ";

        for (var i = 0; i < Controller.Bones.Count; i++)
            output += WriteOutput(Controller.Bones[i]);

        for (var i = 0; i < Constraints.Count; i++)
            output += WriteOutput(Constraints[i]);

        File.WriteAllText(Controller.AnimationFilename+fileExt, output);
    }

    public string WriteOutput(SmoothMotionBone bone)
    {
        var output = "";
        for (int frame = 0; frame < Controller.NumFrames; frame++)
            output += WriteVector(bone.GetFrameInfo(frame).Position);
        return output;
    }

    public string WriteOutput(PlaneCollisionConstraint constraint)
    {
        return WriteVector(constraint.Plane.transform.position)
            + WriteVector(constraint.Plane.transform.TransformDirection(constraint.LocalNormal));
    }

    private string WriteVector(Vector3 vector)
    {
        return WriteFloat(vector.x) + WriteFloat(vector.y) + WriteFloat(vector.z);
    }

    private string WriteFloat(float num)
    {
        return "" + num + " ";
    }
}
