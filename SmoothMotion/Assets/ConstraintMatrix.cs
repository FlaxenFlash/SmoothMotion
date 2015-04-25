using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

public class ConstraintMatrix
{
    public float[,,] Seperations;
        
    public ConstraintMatrix(List<SmoothMotionBone> bones, int frames)
    {
        Seperations = new float[bones.Count, bones.Count, frames];

        for(var frame = 0; frame < frames; frame++)
        {
            for(var x = 0 ; x < bones.Count; x++)
            {
                for (var y = 0; y < bones.Count; y++)
                {
                    var boneX = bones[x];
                    var boneY = bones[y];
                    boneX.SetFrame(frame);
                    boneY.SetFrame(frame);

                    Seperations[x, y, frame] = (boneX.transform.position - boneY.transform.position).magnitude;
                }
            }
        }
    }
}
