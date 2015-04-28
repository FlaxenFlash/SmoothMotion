using System.Collections.Generic;
using UnityEngine;


public class SmoothMotionController : MonoBehaviour 
{
    public List<SmoothMotionBone> Bones;
    public Animation Animator;
    public int SamplingFPS;
    public Transform SkeletonRoot;
    public string AnimationFilename;
    public int NumFrames;

	void Start()
	{
        Bones = new List<SmoothMotionBone>();
	    TagBonesDeep(SkeletonRoot);
        GetBonePositions();
	}

    private void GetBonePositions()
    {
        float frameTime = 1f / SamplingFPS;

        foreach (AnimationState state in Animator)
        {
            state.speed = 0;
            state.time = 0;
            NumFrames = 0;

            while (state.time < state.length)
            {
                Animator.Sample();

                foreach (var bone in Bones)
                {
                    bone.SaveFramePosition();
                }
                state.time += frameTime;
                NumFrames++;
            }

            //Only want the first animation
            break;
        }
    }

    private void TagBonesDeep(Transform node)
    {
        node.gameObject.AddComponent<SmoothMotionBone>();
        Bones.Add(node.GetComponent<SmoothMotionBone>());
        foreach (Transform child in node)
        {
            TagBonesDeep(child);
        }
    }
}
