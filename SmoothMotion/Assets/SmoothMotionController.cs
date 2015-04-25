using System.Collections.Generic;
using UnityEngine;


public class SmoothMotionController : MonoBehaviour 
{
    private List<SmoothMotionBone> _bones;
    public Animation Animator;
    public int SamplingFPS;
    public Transform SkeletonRoot;

    private float _frame;
    private int _numFrames;
    private ConstraintMatrix _constraints;

	void Start()
	{
        _bones = new List<SmoothMotionBone>();
	    TagBonesDeep(SkeletonRoot);

	    GetBonePositions();

        _constraints = new ConstraintMatrix(_bones, _numFrames);
	}

    private void GetBonePositions()
    {
        float frameTime = 1f/SamplingFPS;

        foreach (AnimationState state in Animator)
        {
            state.speed = 0;
            state.time = 0;
            _numFrames = 0;

            while (state.time < state.length)
            {
                Animator.Sample();

                foreach (var bone in _bones)
                {
                    bone.SaveFramePosition();
                }
                state.time += frameTime;
                _numFrames++;
            }

            //Only want the first animation
            break;
        }
    }

    private void TagBonesDeep(Transform node)
    {
        node.gameObject.AddComponent<SmoothMotionBone>();
        _bones.Add(node.GetComponent<SmoothMotionBone>());
        foreach (Transform child in node)
        {
            TagBonesDeep(child);
        }
    }

    void Update()
    {
        _frame += Time.deltaTime*SamplingFPS;
        foreach (var bone in _bones)
        {
            bone.SetFrame(_frame);
        }
    }
}
