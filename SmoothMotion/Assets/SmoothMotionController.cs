using System.Collections.Generic;
using UnityEngine;


public class SmoothMotionController : MonoBehaviour 
{
    public List<SmoothMotionBone> Bones { get; set; }
    public Animation Animator;
    public int SamplingFPS;
    public Transform SkeletonRoot;
    public string AnimationFilename;
    public int NumFrames{get;set;}
	public bool Initialised { get; set; }

	void Start()
	{
        Bones = new List<SmoothMotionBone>();
	    TagBonesDeep(SkeletonRoot);
        GetBonePositions();
		Initialised = true;
	}

    private void GetBonePositions()
    {
		float frameTime = 1f / SamplingFPS;
	    NumFrames = 0;

		var clip = Animator.clip;
	    var time = 0f;
		while (time < clip.length)
		{
			clip.SampleAnimation(gameObject, time);

			foreach (var bone in Bones)
			{
				bone.SaveFramePosition();
			}
			time += frameTime;
			NumFrames++;
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
