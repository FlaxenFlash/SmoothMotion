﻿using System.Collections.Generic;
using UnityEngine;

public struct FrameInfo
{
    public Vector3 Position;
    public Vector3 Scale;
    public Quaternion Rotation;
}

public class SmoothMotionBone : MonoBehaviour
{
    private List<FrameInfo> _frames;

    public FrameInfo GetFrameInfo(int frame)
    {
        return _frames[frame % _frames.Count];
    }

    public void SetFrame(float frame)
    {
        frame = frame%_frames.Count;
        var previous = _frames[(int) Mathf.Floor(frame)];
        var next = _frames[(int)Mathf.Ceil(frame) % _frames.Count];

        var lerpFraction = frame%1;
        transform.position = Vector3.Lerp(previous.Position, next.Position, lerpFraction);
        transform.rotation = Quaternion.Lerp(previous.Rotation, next.Rotation, lerpFraction);
        transform.localScale = Vector3.Lerp(previous.Scale, next.Scale, lerpFraction);
    }

    public void SaveFramePosition()
    {
        if (_frames == null)
            _frames = new List<FrameInfo>();

        _frames.Add(CreateFrameInfo());
    }

    public void OverridePosition(Vector3 position, int frame)
    {
        if(frame >= _frames.Count)
        {
            Debug.LogError("Failed to override bone position, exceeded framecount");
            return;
        }

        var frameinfo = _frames[frame];
        frameinfo.Position = position;
        _frames[frame] = frameinfo;
    }

    public FrameInfo CreateFrameInfo()
    {
        var frame = new FrameInfo();
        frame.Position = transform.position;
        frame.Scale = transform.localScale;
        frame.Rotation = transform.rotation;

        return frame;
    }
}
